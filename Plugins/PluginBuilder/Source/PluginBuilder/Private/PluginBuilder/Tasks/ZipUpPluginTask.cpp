// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Tasks/ZipUpPluginTask.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "HAL/PlatformFileManager.h"
#include "Editor.h"

namespace PluginBuilder
{
	FZipUpPluginTask::FZipUpPluginTask(
		const FString& InEngineVersion,
		const FUATBatchFileParams& InUATBatchFileParams,
		const FZipUpPluginParams& InZipUpPluginParams
	)
		: IUATBatchFileTask(InEngineVersion, InUATBatchFileParams)
		, ZipUpPluginParams(InZipUpPluginParams)
	{
	}
	
	void FZipUpPluginTask::Initialize()
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		
		if (ZipUpPluginParams.bKeepUPluginProperties)
		{
			if (!CopyUPluginProperties())
			{
				UE_LOG(LogPluginBuilder, Error, TEXT("Failed to copy uplugin file."));
			}
		}
		
		PlatformFile.CreateDirectoryTree(*GetZipTempDirectoryPath());
		PlatformFile.CopyDirectoryTree(*GetZipTempDirectoryPath(), *GetBuiltPluginDestinationPath(), true);
						
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
			const FString DirectoryPathToDelete = GetZipTempDirectoryPath() / DirectoryNameToDelete;
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
			bHasAnyError = true;
			State = EState::PreTerminate;
			return;
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
			TEXT("-nocompile"),
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
			GetDestinationDirectoryName() /
			UATBatchFileParams.GetPluginNameInSpecifiedFormat()
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
