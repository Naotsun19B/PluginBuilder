// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Tasks/PackagePluginTask.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/Utilities/ZipUtils.h"
#include "PluginBuilder/Types/EngineVersions.h"
#include "DesktopPlatformModule.h"
#include "HAL/PlatformFileManager.h"
#include "Framework/Application/SlateApplication.h"

namespace PluginBuilder
{
	DECLARE_STATS_GROUP(TEXT("PackagePluginTask"), STATGROUP_PackagePluginTask, STATCAT_Advanced);
	
	FPackagePluginArgs::FPackagePluginArgs()
	{
		const auto& Settings = UPluginBuilderSettings::Get();
		if (Settings.SelectedBuildTarget.IsSet())
		{
			const FBuildTarget& BuildTarget = Settings.SelectedBuildTarget.GetValue();
			PluginName = BuildTarget.GetPluginName();
			PluginVersionName = BuildTarget.GetPluginVersionName();
			bCanContainContent = BuildTarget.CanContainContent();
			UPluginFile = BuildTarget.GetUPluginFile();
		}
		EngineVersions = Settings.EngineVersions;
		bStopPackagingProcessImmediately = Settings.bStopPackagingProcessImmediately;
		TargetPlatforms = Settings.TargetPlatforms;
		bRocket = Settings.bRocket;
		bCreateSubFolder = Settings.bCreateSubFolder;
		bStrictIncludes = Settings.bStrictIncludes;
		bZipUp = Settings.bZipUp;

		if (!Settings.bSelectOutputDirectoryManually)
		{
			OutputDirectoryPath = Settings.OutputDirectoryPath.Path;
		}
	}

	FPackagePluginArgs::FPackagePluginArgs(
		const FString& InPluginName,
		const FString& InPluginVersionName,
		const bool bInCanContainContent,
		const FString& InUPluginFile,
		const TArray<FString>& InEngineVersions,
		const FString& InOutputDirectoryPath,
		const bool bInStopPackagingProcessImmediately /* = false */,
		const TArray<FString>& InTargetPlatforms /* = TArray<FString>{} */,
		const bool bInRocket /* = true */,
		const bool bInCreateSubFolder /* = false */,
		const bool bInStrictIncludes /* = false */,
		const bool bInZipUp /* = true */
	)
		: PluginName(InPluginName)
		, PluginVersionName(InPluginVersionName)
		, bCanContainContent(bInCanContainContent)
		, UPluginFile(InUPluginFile)
		, EngineVersions(InEngineVersions)
		, OutputDirectoryPath(InOutputDirectoryPath)
		, bStopPackagingProcessImmediately(bInStopPackagingProcessImmediately)
		, TargetPlatforms(InTargetPlatforms)
		, bRocket(bInRocket)
		, bCreateSubFolder(bInCreateSubFolder)
		, bStrictIncludes(bInStrictIncludes)
		, bZipUp(bInZipUp)
	{
	}

	TSharedRef<FPackagePluginTask> FPackagePluginTask::CreateTask(const FPackagePluginArgs& InArgs)
	{
		TSharedRef<FPackagePluginTask> NewTask = MakeShared<FPackagePluginTask>();
		NewTask->Args = InArgs;
		return NewTask;
	}

	void FPackagePluginTask::CleanUp()
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		PlatformFile.DeleteDirectoryRecursively(*GetZipTempDirectoryPath());
	}

	TStatId FPackagePluginTask::GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FPackagePluginTask, STATGROUP_PackagePluginTask);
	}

	bool FPackagePluginTask::IsTickable() const
	{
		return bStartedTask;
	}

	void FPackagePluginTask::Tick(float DeltaTime)
	{
		if (FPlatformProcess::IsProcRunning(ProcessHandle))
		{
			const FString OutputLog = FPlatformProcess::ReadPipe(ReadPipe);
			if (!OutputLog.IsEmpty())
			{
				TArray<FString> Lines;
				OutputLog.ParseIntoArrayLines(Lines);
				for (const auto& Line : Lines)
				{
					UE_LOG(LogPluginBuilder, Log, TEXT("%s"), *Line);
				}
			}

			if (bNeedToCancel && Args.bStopPackagingProcessImmediately)
			{
				FPlatformProcess::TerminateProc(ProcessHandle);
			}
		}
		else 
		{
			if (ProcessingIndex >= 0)
			{
				IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				
				int32 ReturnCode = -1;
				FPlatformProcess::GetProcReturnCode(ProcessHandle, &ReturnCode);
				const FString BuiltPluginDestinationPath = GetBuiltPluginDestinationPath();
				
				UE_LOG(LogPluginBuilder, Log, TEXT("----------------------------------------------------------------------------------------------------"));
				UE_LOG(LogPluginBuilder, Log, TEXT("[Return Code] %d"), ReturnCode);
				if (ReturnCode == 0 && !bNeedToCancel)
				{
					UE_LOG(LogPluginBuilder, Log, TEXT("[Output Directory] %s"), *BuiltPluginDestinationPath);

					if (Args.bZipUp)
					{
						const FString ZipTempDirectoryPath = (
							GetZipTempDirectoryPath() / GetDestinationDirectoryName() / Args.PluginName
						);
						PlatformFile.CreateDirectoryTree(*ZipTempDirectoryPath);
						PlatformFile.CopyDirectoryTree(*ZipTempDirectoryPath, *BuiltPluginDestinationPath, true);
						
						TArray<FString> DirectoryNamesToDelete = { TEXT("Binaries"), TEXT("Intermediate") };
						if (!Args.bCanContainContent)
						{
							DirectoryNamesToDelete.Add(TEXT("Content"));
						}
						for (const auto& DirectoryNameToDelete : DirectoryNamesToDelete)
						{
							const FString DirectoryPathToDelete = ZipTempDirectoryPath / DirectoryNameToDelete;
							PlatformFile.DeleteDirectoryRecursively(*DirectoryPathToDelete);
						}

						const FString ZipFileName = FString::Printf(TEXT("%s%s.zip"), *Args.PluginName, *Args.PluginVersionName);
						const FString ZipFilePath = (
							GetPackagedPluginDestinationPath() / GetDestinationDirectoryName() / ZipFileName
						);
						if (PlatformFile.FileExists(*ZipFilePath))
						{
							PlatformFile.DeleteFile(*ZipFilePath);
						}
						
						if (FZipUtils::ZipUp(ZipTempDirectoryPath, ZipFilePath))
						{
							UE_LOG(LogPluginBuilder, Log, TEXT("[Zip File] %s"), *ZipFilePath);
						}
						else
						{
							UE_LOG(LogPluginBuilder, Error, TEXT("Failed to create the zip file."));
							bHasAnyError = true;
						}
					}
				}
				else
				{
					PlatformFile.DeleteDirectoryRecursively(*BuiltPluginDestinationPath);
					bHasAnyError = true;
				}

				FPlatformProcess::CloseProc(ProcessHandle);
			}

			ProcessingIndex++;
			
			if (Args.EngineVersions.IsValidIndex(ProcessingIndex) && !bNeedToCancel)
			{
				const FString EngineVersion = Args.EngineVersions[ProcessingIndex];
				
				FString UATBatchFile;
				if (!FEngineVersions::FindUATBatchFileByVersionName(EngineVersion, UATBatchFile))
				{
					OnTaskFinished.ExecuteIfBound(false, false);
					return;
				}

				UE_LOG(LogPluginBuilder, Log, TEXT("===================================================================================================="));
				UE_LOG(LogPluginBuilder, Log, TEXT("[Plugin Name] %s / [Plugin Version] %s / [Engine Version] %s"), *Args.PluginName, *Args.PluginVersionName, *EngineVersion);
				UE_LOG(LogPluginBuilder, Log, TEXT("----------------------------------------------------------------------------------------------------"));

				TArray<FString> Params = {
					TEXT("BuildPlugin"),
					FString::Printf(TEXT("-Plugin=\"%s\""), *Args.UPluginFile),
					FString::Printf(TEXT("-Package=\"%s\""), *GetBuiltPluginDestinationPath())
				};
				if (Args.TargetPlatforms.Num() > 0)
				{
					Params.Add(
						FString::Printf(
								TEXT("-targetplatform=\"%s\""),
								*FString::Join(Args.TargetPlatforms, TEXT(",")
							)
						)
					);
				}
				if (Args.bRocket)
				{
					Params.Add(TEXT("-Rocket"));
				}
				if (Args.bCreateSubFolder)
				{
					Params.Add(TEXT("-CreateSubFolder"));
				}
				if (Args.bStrictIncludes)
				{
					Params.Add(TEXT("-strictincludes"));
				}
				
				void* WritePipe = nullptr;
				FPlatformProcess::CreatePipe(ReadPipe, WritePipe);
				ProcessHandle = FPlatformProcess::CreateProc(
					*UATBatchFile,
					*FString::Join(Params, TEXT(" ")),
					false,
					true,
					true,
					nullptr,
					0,
					nullptr,
					WritePipe,
					ReadPipe
				);
			}
			else
			{
				OnTaskFinished.ExecuteIfBound(!bHasAnyError, bNeedToCancel);
			}
		}
	}

	bool FPackagePluginTask::IsTickableWhenPaused() const
	{
		return true;
	}

	bool FPackagePluginTask::IsTickableInEditor() const
	{
		return true;
	}

	const FPackagePluginArgs& FPackagePluginTask::GetArgs() const
	{
		return Args;
	}

	void FPackagePluginTask::StartProcess(const FOnTaskFinished& Callback)
	{
		OnTaskFinished = Callback;
		
		if (!Args.OutputDirectoryPath.IsSet())
		{
			if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
			{
				FString OutputDirectoryPath;
				const bool bWasSelected = DesktopPlatform->OpenDirectoryDialog(
					FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
					TEXT("Select Output Directory"),
					FPaths::ProjectDir(),
					OutputDirectoryPath
				);
				if (bWasSelected)
				{
					Args.OutputDirectoryPath = OutputDirectoryPath;
				}
			}
		}

		if (!Args.OutputDirectoryPath.IsSet())
		{
			OnTaskFinished.ExecuteIfBound(true, true);
			return;
		}

		CleanUp();
		bStartedTask = true;
	}

	void FPackagePluginTask::RequestCancelProcess()
	{
		bNeedToCancel = true;
	}

	FString FPackagePluginTask::GetDestinationDirectoryName() const
	{
		if (!Args.EngineVersions.IsValidIndex(ProcessingIndex))
		{
			return {};
		}

		return FString::Printf(TEXT("%s_%s"), *Args.PluginName, *Args.EngineVersions[ProcessingIndex]);
	}

	FString FPackagePluginTask::GetBuiltPluginDestinationPath() const
	{
		return Args.OutputDirectoryPath.Get(FPaths::ProjectDir()) / TEXT("BuiltPlugins") / GetDestinationDirectoryName();
	}

	FString FPackagePluginTask::GetPackagedPluginDestinationPath() const
	{
		return Args.OutputDirectoryPath.Get(FPaths::ProjectDir()) / TEXT("PackagedPlugins");
	}

	FString FPackagePluginTask::GetZipTempDirectoryPath()
	{
		return FString(FPlatformProcess::UserTempDir()) / TEXT("PluginBuilder");
	}
}
