// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TickableEditorObject.h"
#include "PluginBuilder/Types/PackagePluginParams.h"

namespace PluginBuilder
{
	/**
	 * A task class that packages plugins.
	 */
	class PLUGINBUILDER_API FPackagePluginTask
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
		FPackagePluginTask(const FString& InEngineVersion, const FBuildPluginParams& InParams);

		// Returns the types of task progress states.
		EState GetState() const;

		// Returns whether any error occurred during the packaging process.
		bool HasAnyError() const;
		
		// Called only once when task processing starts.
		void Initialize();

		// Called every frame while the task is being processed.
		void Tick(float DeltaTime);

		// Called only once when task processing ends.
		void Terminate();

		// Requests cancellation of the task.
		void RequestCancel();
		
		// Functions that returns the path of a directory or working directory that
		// outputs pre-built or packaged plugins.
		FString GetDestinationDirectoryName() const;
		FString GetBuiltPluginDestinationPath() const;
		FString GetPackagedPluginDestinationPath() const;
		static FString GetZipTempDirectoryPath();

	private:
		// Copies the properties of the original uplugin file to the UAT output uplugin file.
		bool CopyUPluginProperties() const;
		
	private:
		// The engine version to build for this task
		FString EngineVersion;
		
		// The dataset used to process plugin build.
		FBuildPluginParams Params;

		// The task progress state.
		EState State;

		// Whether any error occurred during the packaging process.
		bool bHasAnyError;

		// The process handle of the batch file.
		FProcHandle ProcessHandle;
		
		// The read pipe for outputting from the standard output of a batch file to the output log.
		void* ReadPipe;
	};
}
