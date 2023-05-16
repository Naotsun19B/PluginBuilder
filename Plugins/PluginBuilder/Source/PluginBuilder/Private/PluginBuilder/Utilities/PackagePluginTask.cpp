// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PackagePluginTask.h"
#include "PluginBuilder/Utilities/ZipUtils.h"
#include "PluginBuilder/Types/EngineVersions.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "HAL/PlatformFileManager.h"
#include "Editor.h"

namespace PluginBuilder
{
	FPackagePluginTask::FPackagePluginTask(const FString& InEngineVersion, const FBuildPluginParams& InParams)
		: EngineVersion(InEngineVersion)
		, Params(InParams)
		, State(EState::PreInitialize)
		, bHasAnyError(false)
		, ReadPipe(nullptr)
	{
	}

	FPackagePluginTask::EState FPackagePluginTask::GetState() const
	{
		return State;
	}

	bool FPackagePluginTask::HasAnyError() const
	{
		return bHasAnyError;
	}
	
	void FPackagePluginTask::Initialize()
	{
		FString UATBatchFile;
		if (!FEngineVersions::FindUATBatchFileByVersionName(EngineVersion, UATBatchFile))
		{
			UE_LOG(LogPluginBuilder, Error, TEXT("Could not find UAT batch file. (Engine Version = %s)"), *EngineVersion);
			bHasAnyError = true;
			State = EState::Terminated;
			return;
		}
		
		UE_LOG(LogPluginBuilder, Log, TEXT("===================================================================================================="));
		UE_LOG(LogPluginBuilder, Log, TEXT("[Plugin Name] %s / [Plugin Version] %s / [Engine Version] %s"), *Params.GetPluginNameInSpecifiedFormat(), *Params.PluginVersionName, *EngineVersion);
		UE_LOG(LogPluginBuilder, Log, TEXT("----------------------------------------------------------------------------------------------------"));

		TArray<FString> Arguments = {
			TEXT("BuildPlugin"),
			FString::Printf(TEXT("-Plugin=\"%s\""), *Params.UPluginFile),
			FString::Printf(TEXT("-Package=\"%s\""), *GetBuiltPluginDestinationPath())
		};
		if (Params.TargetPlatforms.Num() > 0)
		{
			Arguments.Add(
				FString::Printf(
						TEXT("-targetplatform=\"%s\""),
						*FString::Join(Params.TargetPlatforms, TEXT(",")
					)
				)
			);
		}
		if (Params.bRocket)
		{
			Arguments.Add(TEXT("-Rocket"));
		}
		if (Params.bCreateSubFolder)
		{
			Arguments.Add(TEXT("-CreateSubFolder"));
		}
		if (Params.bStrictIncludes)
		{
			Arguments.Add(TEXT("-strictincludes"));
		}
				
		void* WritePipe = nullptr;
		FPlatformProcess::CreatePipe(ReadPipe, WritePipe);
		ProcessHandle = FPlatformProcess::CreateProc(
			*UATBatchFile,
			*FString::Join(Arguments, TEXT(" ")),
			false,
			true,
			true,
			nullptr,
			0,
			nullptr,
			WritePipe,
			ReadPipe
		);

		State = EState::Processing;
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
		}
		else
		{
			enum EReturnCode
			{
				RC_ProcessDidNotRun = -1,
				RC_DirectoryDoesNotExists = -2,
			};
			
			int32 ReturnCode;
			if (!FPlatformProcess::GetProcReturnCode(ProcessHandle, &ReturnCode))
			{
				ReturnCode = RC_ProcessDidNotRun;
			}
			else
			{
				IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				if (!PlatformFile.DirectoryExists(*GetBuiltPluginDestinationPath()))
				{
					ReturnCode = RC_DirectoryDoesNotExists;
				}
			}
			
			UE_LOG(LogPluginBuilder, Log, TEXT("----------------------------------------------------------------------------------------------------"));
			if (ReturnCode == 0)
			{
				UE_LOG(LogPluginBuilder, Log, TEXT("[Return Code] %d"), ReturnCode);
				UE_LOG(LogPluginBuilder, Log, TEXT("[Output Directory] %s"), *GetBuiltPluginDestinationPath());
			}
			else
			{
				if (ReturnCode == RC_ProcessDidNotRun)
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("UAT process did not run."));
				}
				else if (ReturnCode == RC_DirectoryDoesNotExists)
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("Built plugin directory does not exist. (%s)"), *GetBuiltPluginDestinationPath());
				}
				else
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("[Return Code] %d"), ReturnCode);
				}
				
				bHasAnyError = true;
			}

			State = EState::PreTerminate;
		}
	}

	void FPackagePluginTask::Terminate()
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		
		if (bHasAnyError)
		{
			PlatformFile.DeleteDirectoryRecursively(*GetBuiltPluginDestinationPath());
		}
		else if (Params.bZipUp)
		{
			if (Params.bKeepUPluginProperties)
			{
				if (!CopyUPluginProperties())
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("Failed to copy uplugin file."));
				}
			}
			
			const FString ZipTempDirectoryPath = (
				GetZipTempDirectoryPath() / GetDestinationDirectoryName() / Params.GetPluginNameInSpecifiedFormat()
			);
			PlatformFile.CreateDirectoryTree(*ZipTempDirectoryPath);
			PlatformFile.CopyDirectoryTree(*ZipTempDirectoryPath, *GetBuiltPluginDestinationPath(), true);
						
			TArray<FString> DirectoryNamesToDelete = { TEXT("Intermediate") };
			if (!Params.bKeepBinariesFolder)
			{
				DirectoryNamesToDelete.Add(TEXT("Binaries"));
			}
			if (!Params.bCanPluginContainContent)
			{
				DirectoryNamesToDelete.Add(TEXT("Content"));
			}
			for (const auto& DirectoryNameToDelete : DirectoryNamesToDelete)
			{
				const FString DirectoryPathToDelete = ZipTempDirectoryPath / DirectoryNameToDelete;
				PlatformFile.DeleteDirectoryRecursively(*DirectoryPathToDelete);
			}
			
			FString ZipFilePath;
			if (Params.bOutputAllZipFilesToSingleFolder)
			{
				const FString ZipFileName = FString::Printf(
					TEXT("%s_%s.zip"),
					*GetDestinationDirectoryName(),
					*Params.PluginVersionName
				);
				ZipFilePath = (
					GetPackagedPluginDestinationPath() / ZipFileName
				);
			}
			else
			{
				const FString ZipFileName = FString::Printf(
					TEXT("%s%s.zip"),
					*Params.GetPluginNameInSpecifiedFormat(),
					*Params.PluginVersionName
				);
				ZipFilePath = (
					GetPackagedPluginDestinationPath() /  GetDestinationDirectoryName() / ZipFileName
				);
			}
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

		if (!Params.IsFormatExpectedByMarketplace())
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("The created package is not in a format that can be submitted to the marketplace."));
			UE_LOG(LogPluginBuilder, Warning, TEXT("If you plan to submit to the marketplace, please review the build options and zip up options."));
		}

		FPlatformProcess::CloseProc(ProcessHandle);

		State = EState::Terminated;
	}

	void FPackagePluginTask::RequestCancel()
	{
		if (Params.bStopPackagingProcessImmediately)
		{
			FPlatformProcess::TerminateProc(ProcessHandle);
			State = EState::Terminated;
		}
	}

	FString FPackagePluginTask::GetDestinationDirectoryName() const
	{
		return FString::Printf(TEXT("%s_%s"), *Params.GetPluginNameInSpecifiedFormat(), *EngineVersion);
	}

	FString FPackagePluginTask::GetBuiltPluginDestinationPath() const
	{
		return (Params.OutputDirectoryPath.Get(FPaths::ProjectDir()) / TEXT("BuiltPlugins") / GetDestinationDirectoryName());
	}

	FString FPackagePluginTask::GetPackagedPluginDestinationPath() const
	{
		return (Params.OutputDirectoryPath.Get(FPaths::ProjectDir()) / TEXT("PackagedPlugins"));
	}

	FString FPackagePluginTask::GetZipTempDirectoryPath()
	{
		return (FString(FPlatformProcess::UserTempDir()) / TEXT("PluginBuilder"));
	}

	bool FPackagePluginTask::CopyUPluginProperties() const
	{
		const FString& OriginalUPluginFile = Params.UPluginFile;
		const FString& OutputUPluginFile = (GetBuiltPluginDestinationPath() / FPaths::GetCleanFilename(Params.UPluginFile));
		
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
