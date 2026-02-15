// Copyright 2022-2026 Naotsun. All Rights Reserved.

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

		if(!CopyPluginFilter())
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("Failed to copy Config folder."));
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
		
		const FString PluginDisplayName = (
			ZipUpPluginParams.bOutputAllZipFilesToSingleFolder ?
			GetDestinationDirectoryName() :
			UATBatchFileParams.GetPluginNameInSpecifiedFormat()
		);

		const FString Separator = (ZipUpPluginParams.bOutputAllZipFilesToSingleFolder ? TEXT("_") : TEXT(""));

		const FString EngineVersionSuffix = (
			ZipUpPluginParams.bAppendEngineVersionToZipFileName ?
			FString::Printf(TEXT("-UE%s"), *EngineVersion) : 
			TEXT("")
		);

		const FString ZipFileName = FString::Printf(
			TEXT("%s%s%s%s.zip"),
			*PluginDisplayName,
			*Separator,
			*UATBatchFileParams.PluginVersionName,
			*EngineVersionSuffix
		);

		const FString ZipDirectoryPath = (
			ZipUpPluginParams.bOutputAllZipFilesToSingleFolder ? 
			GetPackagedPluginDestinationPath() : 
			(GetPackagedPluginDestinationPath() / GetDestinationDirectoryName())
		);

		ZipFilePath = (ZipDirectoryPath / ZipFileName);
		
		if (PlatformFile.FileExists(*ZipFilePath))
		{
			PlatformFile.DeleteFile(*ZipFilePath);
		}

		UE_LOG(LogPluginBuilder, Log, TEXT("----------------------------------------------------------------------------------------------------"));
		UE_LOG(LogPluginBuilder, Log, TEXT("[Zip File] %s"), *ZipFilePath);
		
		if (!ZipUpPluginParams.bAppendEngineVersionToZipFileName)
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("When submitting to Fab, if the zip files for each engine version have the same name, the person in charge may ask you to resubmit it, saying, ``Please make sure that the engine version can be determined from the file name.''"));
		}
		
		IUATBatchFileTask::Initialize();
	}

	void FZipUpPluginTask::Terminate()
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		PlatformFile.DeleteDirectoryRecursively(*GetZipTempDirectoryPath());
		
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
			TEXT(UE_PLUGIN_NAME) /
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

	bool FZipUpPluginTask::CopyPluginFilter() const
	{
		const FString& OriginalConfigFolder = FPaths::GetPath(UATBatchFileParams.UPluginFile) / TEXT("Config");
		const FString& OutputConfigFolder = (GetBuiltPluginDestinationPath() / TEXT("Config"));

		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		
		return PlatformFile.CopyDirectoryTree(
			*OutputConfigFolder,
			*OriginalConfigFolder,
			true
		);
	}
}
