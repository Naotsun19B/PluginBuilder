// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/Tasks/UploadToCloudTask.h"
#include "PluginBuilder/Tasks/ZipUpPluginTask.h"
#include "PluginBuilder/CloudStorages/CloudStorageManager.h"
#include "PluginBuilder/CloudStorages/ICloudStorageProvider.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"

namespace PluginBuilder
{
	FUploadToCloudTask::FUploadToCloudTask(
		const TArray<TSharedPtr<FZipUpPluginTask>>& InZipTasks,
		const FString& InPackagedPluginsPath,
		const FString& InPluginName,
		bool bInGetShareUrls
	)
		: ZipTasks(InZipTasks)
		, PackagedPluginsPath(InPackagedPluginsPath)
		, PluginName(InPluginName)
		, bGetShareUrls(bInGetShareUrls)
		, State(EState::PreInitialize)
		, bHasAnyError(false)
		, bHttpRequestPending(false)
		, CurrentFileIndex(0)
		, CurrentFileProgress(0.f)
	{
	}

	FUploadToCloudTask::FUploadToCloudTask(
		const TArray<FString>& InZipFilePaths,
		const FString& InPackagedPluginsPath,
		const FString& InPluginName,
		bool bInGetShareUrls
	)
		: ZipFilePaths(InZipFilePaths)
		, PackagedPluginsPath(InPackagedPluginsPath)
		, PluginName(InPluginName)
		, bGetShareUrls(bInGetShareUrls)
		, State(EState::PreInitialize)
		, bHasAnyError(false)
		, bHttpRequestPending(false)
		, CurrentFileIndex(0)
		, CurrentFileProgress(0.f)
	{
	}

	IPluginBuilderTask::EState FUploadToCloudTask::GetState() const
	{
		return State;
	}

	bool FUploadToCloudTask::HasAnyError() const
	{
		return bHasAnyError;
	}

	FString FUploadToCloudTask::GetTaskLabel() const
	{
		return TEXT("Cloud Storage");
	}

	void FUploadToCloudTask::Initialize()
	{
		Provider = FCloudStorageManager::GetCurrentProvider();
		if (!Provider.IsValid() || !Provider->IsAuthenticated())
		{
			UE_LOG(LogPluginBuilder, Error, TEXT("Cloud Storage upload: Not authenticated. Please sign in from Editor Preferences > Plugins > Plugin Builder - OneDrive."));
			bHasAnyError = true;
			State = EState::Terminated;
			return;
		}

		// Resolve zip file paths from zip tasks if not already set.
		if (ZipFilePaths.IsEmpty())
		{
			for (const TSharedPtr<FZipUpPluginTask>& ZipTask : ZipTasks)
			{
				if (!ZipTask.IsValid() || ZipTask->HasAnyError())
				{
					continue;
				}
				const FString& ZipPath = ZipTask->GetZipFilePath();
				if (!ZipPath.IsEmpty())
				{
					ZipFilePaths.Add(ZipPath);
				}
			}
		}

		if (ZipFilePaths.IsEmpty())
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("Cloud Storage upload: No zip files to upload."));
			State = EState::Terminated;
			return;
		}

		UE_LOG(LogPluginBuilder, Log, TEXT("===================================================================================================="));
		UE_LOG(LogPluginBuilder, Log, TEXT("Cloud Storage upload: Starting upload of %d file(s)..."), ZipFilePaths.Num());

		State = EState::Processing;
		ProcessNextFile();
	}

	void FUploadToCloudTask::Tick(float /* DeltaTime */)
	{
		if (bHttpRequestPending)
		{
			return;
		}

		if (CurrentFileIndex < ZipFilePaths.Num())
		{
			ProcessNextFile();
		}
		else
		{
			State = EState::PreTerminate;
		}
	}

	void FUploadToCloudTask::Terminate()
	{
		FinalizeResults();
		State = EState::Terminated;
	}

	float FUploadToCloudTask::GetProgress() const
	{
		if (ZipFilePaths.IsEmpty())
		{
			return -1.f;
		}
		const float FileProgress = (static_cast<float>(CurrentFileIndex) + FMath::Clamp(CurrentFileProgress, 0.f, 1.f)) / static_cast<float>(ZipFilePaths.Num());
		return FMath::Clamp(FileProgress, 0.f, 1.f);
	}

	FString FUploadToCloudTask::GetProgressText() const
	{
		if (ZipFilePaths.IsEmpty())
		{
			return FString();
		}
		return FString::Printf(TEXT("[%d/%d]"), FMath::Min(CurrentFileIndex + 1, ZipFilePaths.Num()), ZipFilePaths.Num());
	}

	bool FUploadToCloudTask::IsCloudUploadTask() const
	{
		return true;
	}

	void FUploadToCloudTask::ProcessNextFile()
	{
		if (CurrentFileIndex >= ZipFilePaths.Num())
		{
			State = EState::PreTerminate;
			return;
		}

		CurrentFileProgress = 0.f;

		const FString& LocalPath = ZipFilePaths[CurrentFileIndex];
		const FString RemotePath = BuildRemotePath(LocalPath);

		UE_LOG(LogPluginBuilder, Log, TEXT("Cloud Storage upload: [%d/%d] %s"), CurrentFileIndex + 1, ZipFilePaths.Num(), *FPaths::GetCleanFilename(LocalPath));

		bHttpRequestPending = true;

		Provider->UploadFile(
			LocalPath,
			RemotePath,
			[this, LocalPath](bool bSuccess, const FString& ItemId)
			{
				if (!bSuccess || ItemId.IsEmpty())
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("Cloud Storage upload: Failed to upload %s."), *FPaths::GetCleanFilename(LocalPath));
					bHasAnyError = true;
					CurrentFileIndex++;
					bHttpRequestPending = false;
					return;
				}

				SuccessfulUploads.Add(LocalPath);

				if (!bGetShareUrls)
				{
					CurrentFileIndex++;
					bHttpRequestPending = false;
					return;
				}

				UE_LOG(LogPluginBuilder, Log, TEXT("Cloud Storage upload: Upload succeeded. Getting share URL..."));

				Provider->GetShareUrl(
					ItemId,
					[this, LocalPath](bool bUrlSuccess, const FString& ShareUrl)
					{
						if (!bUrlSuccess || ShareUrl.IsEmpty())
						{
							UE_LOG(LogPluginBuilder, Error, TEXT("Cloud Storage upload: Failed to get share URL for %s."), *FPaths::GetCleanFilename(LocalPath));
							bHasAnyError = true;
						}
						else
						{
							ShareUrlResults.Add(LocalPath, ShareUrl);
						}
						CurrentFileIndex++;
						bHttpRequestPending = false;
					}
				);
			},
			[this](float Progress)
			{
				CurrentFileProgress = Progress;
			}
		);
	}

	void FUploadToCloudTask::FinalizeResults()
	{
		UE_LOG(LogPluginBuilder, Log, TEXT("===================================================================================================="));
		UE_LOG(LogPluginBuilder, Log, TEXT("Cloud Storage upload: Results:"));

		for (const FString& FilePath : ZipFilePaths)
		{
			const FString FileName = FPaths::GetCleanFilename(FilePath);

			if (!SuccessfulUploads.Contains(FilePath))
			{
				UE_LOG(LogPluginBuilder, Error, TEXT("  [FAILED] %s"), *FileName);
				continue;
			}

			if (!bGetShareUrls)
			{
				UE_LOG(LogPluginBuilder, Log, TEXT("  [OK] %s"), *FileName);
				continue;
			}

			const FString* ShareUrl = ShareUrlResults.Find(FilePath);
			if (ShareUrl != nullptr && !ShareUrl->IsEmpty())
			{
				UE_LOG(LogPluginBuilder, Log, TEXT("  %s -> %s"), *FileName, **ShareUrl);
			}
			else
			{
				UE_LOG(LogPluginBuilder, Error, TEXT("  [FAILED to get URL] %s"), *FileName);
			}
		}

		if (bGetShareUrls && (ShareUrlResults.Num() > 0))
		{
			WriteShareUrlsToFile();
		}
	}

	FString FUploadToCloudTask::BuildRemotePath(const FString& LocalZipFilePath) const
	{
		// Strip PackagedPlugins prefix to get the relative path.
		FString RelativePath = LocalZipFilePath;
		const FString NormalizedBase = FPaths::ConvertRelativePathToFull(PackagedPluginsPath);
		const FString NormalizedLocal = FPaths::ConvertRelativePathToFull(LocalZipFilePath);
		if (NormalizedLocal.StartsWith(NormalizedBase))
		{
			RelativePath = NormalizedLocal.Mid(NormalizedBase.Len()).TrimChar(TEXT('/'));
		}

		// Build remote path: [BaseFolderPath/]PluginName/RelativePath
		const FString BaseFolderPath = Provider.IsValid() ? Provider->GetRemoteBaseFolderPath() : FString();
		if (BaseFolderPath.IsEmpty())
		{
			return FString::Printf(TEXT("%s/%s"), *PluginName, *RelativePath);
		}
		
		return FString::Printf(TEXT("%s/%s/%s"), *BaseFolderPath, *PluginName, *RelativePath);
	}

	void FUploadToCloudTask::WriteShareUrlsToFile() const
	{
		const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
		const FString OutputDirectory = (FPaths::ProjectSavedDir() / TEXT("PluginBuilder"));
		const FString FilePath = (OutputDirectory / FString::Printf(TEXT("ShareUrls_%s.txt"), *Timestamp));

		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		PlatformFile.CreateDirectoryTree(*OutputDirectory);

		FString Content;
		for (const auto& Result : ShareUrlResults)
		{
			Content += FString::Printf(TEXT("%s -> %s\n"), *FPaths::GetCleanFilename(Result.Key), *Result.Value);
		}

		if (FFileHelper::SaveStringToFile(Content, *FilePath))
		{
			UE_LOG(LogPluginBuilder, Log, TEXT("Cloud Storage upload: Share URL list saved to: %s"), *FilePath);
		}
		else
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("Cloud Storage upload: Failed to write share URL list to: %s"), *FilePath);
		}
	}
}
