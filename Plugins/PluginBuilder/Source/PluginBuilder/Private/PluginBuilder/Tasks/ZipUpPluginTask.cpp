// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Tasks/ZipUpPluginTask.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/PlatformFile.h"
#include "Misc/Paths.h"

namespace PluginBuilder
{
	FZipUpPluginTask::FZipUpPluginTask(
		const FString& InEngineVersion,
		const FUATBatchFileParams& InUATBatchFileParams,
		const FZipUpPluginParams& InZipUpPluginParams,
		const TSharedPtr<IUATBatchFileTask>& DependentTask
	)
		: IUATBatchFileTask(InEngineVersion, InUATBatchFileParams, DependentTask)
		, ZipUpPluginParams(InZipUpPluginParams)
	{
	}
	
	void FZipUpPluginTask::Initialize()
	{
		if (HasDependentTaskSucceeded.IsSet())
		{
			if (!HasDependentTaskSucceeded.GetValue())
			{
				State = EState::Terminated;
				return;
			}
		}
		
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		
		if (ZipUpPluginParams.bKeepUPluginProperties)
		{
			if (!CopyUPluginProperties())
			{
				UE_LOG(LogPluginBuilder, Error, TEXT("Failed to copy uplugin file."));
			}
		}
		
		const FString ZipTempDirectoryPath = GetZipTempDirectoryPath() / UATBatchFileParams.GetPluginNameInSpecifiedFormat();
		
		PlatformFile.CreateDirectoryTree(*ZipTempDirectoryPath);
		PlatformFile.CopyDirectoryTree(*ZipTempDirectoryPath, *GetBuiltPluginDestinationPath(), true);
						
		TArray<FString> DirectoryNamesToDelete = { TEXT("Intermediate") };
		if (!ZipUpPluginParams.bKeepBinariesFolder)
		{
			DirectoryNamesToDelete.Add(TEXT("Binaries"));
		}
		if (!ZipUpPluginParams.bCanPluginContainContent)
		{
			DirectoryNamesToDelete.Add(TEXT("Content"));
		}
		for (const auto& DirectoryNameToDelete : DirectoryNamesToDelete)
		{
			const FString DirectoryPathToDelete = ZipTempDirectoryPath / DirectoryNameToDelete;
			PlatformFile.DeleteDirectoryRecursively(*DirectoryPathToDelete);
		}
		
		if (ZipUpPluginParams.bOutputAllZipFilesToSingleFolder)
		{
			const FString ZipFileName = FString::Printf(
				TEXT("%s_%s.zip"),
				*GetDestinationDirectoryName(),
				*UATBatchFileParams.PluginVersionName
			);
			ZipFilePath = (
				GetPackagedPluginDestinationPath() / ZipFileName
			);
		}
		else
		{
			const FString ZipFileName = FString::Printf(
				TEXT("%s%s.zip"),
				*UATBatchFileParams.GetPluginNameInSpecifiedFormat(),
				*UATBatchFileParams.PluginVersionName
			);
			ZipFilePath = (
				GetPackagedPluginDestinationPath() /  GetDestinationDirectoryName() / ZipFileName
			);
		}
		
		if (PlatformFile.FileExists(*ZipFilePath))
		{
			PlatformFile.DeleteFile(*ZipFilePath);
		}

		UE_LOG(LogPluginBuilder, Log, TEXT("----------------------------------------------------------------------------------------------------"));
		UE_LOG(LogPluginBuilder, Log, TEXT("[Zip File] %s"), *ZipFilePath);
		
		IUATBatchFileTask::Initialize();
	}

	void FZipUpPluginTask::Terminate()
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		PlatformFile.DeleteDirectoryRecursively(*GetZipTempDirectoryPath());
		
		if (!ZipUpPluginParams.IsFormatExpectedByMarketplace())
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("The created package is not in a format that can be submitted to the marketplace."));
			UE_LOG(LogPluginBuilder, Warning, TEXT("If you plan to submit to the marketplace, please review the build options and zip up options."));
		}

		IUATBatchFileTask::Terminate();
	}

	TArray<FString> FZipUpPluginTask::GetUATArguments() const
	{
		return TArray<FString> {
			TEXT("ZipUtils"),
			FString::Printf(TEXT("-archive=\"%s\""), *ZipFilePath),
			FString::Printf(TEXT("-add=\"%s\""), *GetZipTempDirectoryPath()),
			FString::Printf(TEXT("-compression=%u"), FMath::Min(ZipUpPluginParams.CompressionLevel,static_cast<uint8>(9)))
		};
	}

	FString FZipUpPluginTask::GetDestinationDirectoryPath() const
	{
		return GetPackagedPluginDestinationPath();
	}

	FString FZipUpPluginTask::GetZipTempDirectoryPath() const
	{
		return (
			FString(FPlatformProcess::UserTempDir()) /
			Global::PluginName.ToString() /
			GetDestinationDirectoryName()
		);
	}

	bool FZipUpPluginTask::CopyUPluginProperties() const
	{
		const FString& OriginalUPluginFile = UATBatchFileParams.UPluginFile;
		const FString& OutputUPluginFile = (GetBuiltPluginDestinationPath() / FPaths::GetCleanFilename(OriginalUPluginFile));
		
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (!PlatformFile.FileExists(*OriginalUPluginFile) || !PlatformFile.FileExists(*OutputUPluginFile))
		{
			return false;
		}
		
		return PlatformFile.CopyFile(
			*OutputUPluginFile,
			*OriginalUPluginFile
		);
	}
}
