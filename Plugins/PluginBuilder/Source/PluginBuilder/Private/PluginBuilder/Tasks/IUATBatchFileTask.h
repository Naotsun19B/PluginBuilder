// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformProcess.h"
#include "PluginBuilder/Types/PackagePluginParams.h"

namespace PluginBuilder
{
    /**
	 * An interface class for tasks used in the processing of this plugin.
	 */
	class PLUGINBUILDER_API IUATBatchFileTask
	{
	public:
		// An enum class that defines state of task progress.
		enum class EState : uint8
		{
			PreInitialize,
			Processing,
			PreTerminate,
			Terminated,
		};

	public:
		// Constructor.
		IUATBatchFileTask(
			const FString& InEngineVersion, 
			const FUATBatchFileParams& InUATBatchFileParams,
			const TSharedPtr<IUATBatchFileTask>& DependentTask = nullptr
		);

		// Destructor.
		virtual ~IUATBatchFileTask();
		
		// Returns the types of task progress states.
		virtual EState GetState() const;

		// Returns whether any error occurred during the packaging process.
		virtual bool HasAnyError() const;
		
		// Called only once when task processing starts.
		virtual void Initialize();
		
		// Called every frame while the task is being processed.
		virtual void Tick(float DeltaTime);

		// Called only once when task processing ends.
		virtual void Terminate();

		// Requests cancellation of the task.
		virtual void RequestCancel();

	protected:
		// Returns a list of arguments to pass to the UAT batch file.
		virtual TArray<FString> GetUATArguments() const = 0;
		
		// Returns the path of the directory where task results are output.
		virtual FString GetDestinationDirectoryPath() const = 0;
		
		// Functions that returns the path of a directory or working directory that outputs pre-built or packaged plugins.
		FString GetDestinationDirectoryName() const;
		FString GetBuiltPluginDestinationPath() const;
		FString GetPackagedPluginDestinationPath() const;

	private:
		// Called when a dependent task is destroyed.
		void HandleOnDestroy(const bool bHasDependentTaskError);
		
		// An event that tells if there was an error when the task was destroyed.
		DECLARE_DELEGATE_OneParam(FOnDestroy, const bool /* bHasDependentTaskError */);
		FOnDestroy OnDestroy;
		
	protected:
		// The engine version to build for this task
		FString EngineVersion;
		
		// The dataset used to process UAT batch file.
		FUATBatchFileParams UATBatchFileParams;

		// The task progress state.
		EState State;

		// Whether any error occurred during the packaging process.
		bool bHasAnyError;

		// The process handle of the batch file.
		FProcHandle ProcessHandle;
		
		// The read pipe for outputting from the standard output of a batch file to the output log.
		void* ReadPipe;

		// Whether the dependent task completed successfully.
		TOptional<bool> HasDependentTaskSucceeded;
	};
}
