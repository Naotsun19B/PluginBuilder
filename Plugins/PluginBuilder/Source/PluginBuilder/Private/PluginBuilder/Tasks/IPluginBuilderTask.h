// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * An abstract base interface for all tasks used in the processing of this plugin.
	 */
	class PLUGINBUILDER_API IPluginBuilderTask
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
		// Destructor.
		virtual ~IPluginBuilderTask() = default;

		// Returns the task progress state.
		virtual EState GetState() const = 0;

		// Returns whether any error occurred during the task.
		virtual bool HasAnyError() const = 0;

		// Returns a short label for this task used in progress notifications.
		virtual FString GetTaskLabel() const;

		// Called only once when task processing starts.
		virtual void Initialize() = 0;

		// Called every frame while the task is being processed.
		virtual void Tick(float DeltaTime) = 0;

		// Called only once when task processing ends.
		virtual void Terminate() = 0;

		// Requests cancellation of the task.
		virtual void RequestCancel() {}

		// Returns task progress in [0, 1], or a negative value if unknown.
		virtual float GetProgress() const { return -1.f; }

		// Returns a short progress detail string (e.g. "[35/200]"), or empty if unavailable.
		virtual FString GetProgressText() const { return FString(); }

		// Returns true when this task is a plugin build task.
		virtual bool IsBuildTask() const { return false; }

		// Returns true when this task is a zip-up task.
		virtual bool IsZipTask() const { return false; }

		// Returns true when this task is a cloud storage upload task.
		virtual bool IsCloudUploadTask() const { return false; }
	};
}
