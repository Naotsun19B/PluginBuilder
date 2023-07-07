// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Tasks/IUATBatchFileTask.h"
#include "PluginBuilder/Types/EngineVersions.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/PlatformFile.h"
#include "Misc/Paths.h"

namespace PluginBuilder
{
	IUATBatchFileTask::IUATBatchFileTask(
		const FString& InEngineVersion,
		const FUATBatchFileParams& InUATBatchFileParams,
		const TSharedPtr<IUATBatchFileTask>& DependentTask /* = nullptr */
	)
		: EngineVersion(InEngineVersion)
		, UATBatchFileParams(InUATBatchFileParams)
		, State(EState::PreInitialize)
		, bHasAnyError(false)
		, ReadPipe(nullptr)
	{
		if (DependentTask.IsValid())
		{
			DependentTask->OnDestroy.BindRaw(this, &IUATBatchFileTask::HandleOnDestroy);
		}
	}

	IUATBatchFileTask::~IUATBatchFileTask()
	{
		OnDestroy.ExecuteIfBound(bHasAnyError);
	}

	IUATBatchFileTask::EState IUATBatchFileTask::GetState() const
	{
		return State;
	}

	bool IUATBatchFileTask::HasAnyError() const
	{
		return bHasAnyError;
	}

	void IUATBatchFileTask::Initialize()
	{
		FString UATBatchFile;
		if (!FEngineVersions::FindUATBatchFileByVersionName(EngineVersion, UATBatchFile))
		{
			UE_LOG(LogPluginBuilder, Error, TEXT("Could not find UAT batch file. (Engine Version = %s)"), *EngineVersion);
			bHasAnyError = true;
			State = EState::Terminated;
			return;
		}
		
		void* WritePipe = nullptr;
		FPlatformProcess::CreatePipe(ReadPipe, WritePipe);
		ProcessHandle = FPlatformProcess::CreateProc(
			*UATBatchFile,
			*FString::Join(GetUATArguments(), TEXT(" ")),
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

	void IUATBatchFileTask::Tick(float DeltaTime)
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
				if (!PlatformFile.DirectoryExists(*GetDestinationDirectoryPath()))
				{
					ReturnCode = RC_DirectoryDoesNotExists;
				}
			}
			
			UE_LOG(LogPluginBuilder, Log, TEXT("----------------------------------------------------------------------------------------------------"));
			if (ReturnCode == 0)
			{
				UE_LOG(LogPluginBuilder, Log, TEXT("[Return Code] %d"), ReturnCode);
				UE_LOG(LogPluginBuilder, Log, TEXT("[Output Directory] %s"), *GetDestinationDirectoryPath());
			}
			else
			{
				if (ReturnCode == RC_ProcessDidNotRun)
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("UAT process did not run."));
				}
				else if (ReturnCode == RC_DirectoryDoesNotExists)
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("Directory does not exist. (%s)"), *GetDestinationDirectoryPath());
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

	void IUATBatchFileTask::Terminate()
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		
		if (bHasAnyError)
		{
			PlatformFile.DeleteDirectoryRecursively(*GetBuiltPluginDestinationPath());
		}
		
		FPlatformProcess::CloseProc(ProcessHandle);

		State = EState::Terminated;
	}

	void IUATBatchFileTask::RequestCancel()
	{
		if (UATBatchFileParams.bStopPackagingProcessImmediately)
		{
			FPlatformProcess::TerminateProc(ProcessHandle);
			State = EState::Terminated;
		}
	}

	FString IUATBatchFileTask::GetDestinationDirectoryName() const
	{
		return FString::Printf(TEXT("%s_%s"), *UATBatchFileParams.GetPluginNameInSpecifiedFormat(), *EngineVersion);
	}

	FString IUATBatchFileTask::GetBuiltPluginDestinationPath() const
	{
		return (UATBatchFileParams.OutputDirectoryPath.Get(FPaths::ProjectDir()) / TEXT("BuiltPlugins") / GetDestinationDirectoryName());
	}

	FString IUATBatchFileTask::GetPackagedPluginDestinationPath() const
	{
		return (UATBatchFileParams.OutputDirectoryPath.Get(FPaths::ProjectDir()) / TEXT("PackagedPlugins"));
	}

	void IUATBatchFileTask::HandleOnDestroy(const bool bHasDependentTaskError)
	{
		HasDependentTaskSucceeded = !bHasDependentTaskError;
	}
}
