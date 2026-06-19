// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformProcess.h"
#include "PluginBuilder/Tasks/IPluginBuilderTask.h"
#include "PluginBuilder/Types/PackagePluginParams.h"

namespace PluginBuilder
{
    /**
	 * A base class for tasks that execute UAT batch files.
	 */
	class PLUGINBUILDER_API IUATBatchFileTask : public IPluginBuilderTask
	{
	public:
		// Constructor.
		IUATBatchFileTask(
			const FString& InEngineVersion,
			const FUATBatchFileParams& InUATBatchFileParams,
			const TSharedPtr<IUATBatchFileTask>& DependentTask = nullptr
		);

		// Destructor.
		virtual ~IUATBatchFileTask() override;

		// IPluginBuilderTask interface.
		virtual EState GetState() const override;
		virtual bool HasAnyError() const override;
		virtual FString GetTaskLabel() const override;
		virtual void Initialize() override;
		virtual void Tick(float DeltaTime) override;
		virtual void Terminate() override;
		virtual void RequestCancel() override;
		virtual float GetProgress() const override;
		virtual FString GetProgressText() const override;
		// End of IPluginBuilderTask interface.

		// Returns the engine version for this task.
		FString GetEngineVersion() const;

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

		// Total number of compile actions reported by UBT. 0 means unknown.
		int32 TotalActions;

		// Number of compile actions completed so far.
		int32 CompletedActions;
	};
}
