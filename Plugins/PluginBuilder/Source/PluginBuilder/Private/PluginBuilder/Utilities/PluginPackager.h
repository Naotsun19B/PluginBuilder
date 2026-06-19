// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "PluginBuilder/Types/PackagePluginParams.h"
#include "PluginBuilder/Utilities/EditorNotification.h"

namespace PluginBuilder
{
	class IPluginBuilderTask;
	class FZipUpPluginTask;
	
	/**
	 * A class that handles the packaging of plugins.
	 */
	class PLUGINBUILDER_API FPluginPackager : public FTickableGameObject
	{
	public:
		// Creates and starts a task to specify a parameters and package the plugin.
		// If you don't specify parameters, it will be created from the values set in the editor preferences.
		// Returns whether the package plugin task has started.
		static bool StartPackagePluginTask(const TOptional<FPackagePluginParams>& InParams = {});

		// Creates and starts a task that runs only the build step (no zip, no upload).
		static bool StartBuildOnlyTask();

		// Creates and starts a task that runs only the zip step against existing build output (no build, no upload).
		static bool StartZipOnlyTask();

		// Creates and starts a standalone upload task for existing zip files.
		// InZipFilePaths: absolute paths to the zip files to upload.
		// InPackagedPluginsPath: the PackagedPlugins root used to compute relative remote paths.
		// InPluginName: name used as the top-level folder on cloud storage.
		// bInGetShareUrls: whether to retrieve a share URL for each uploaded file.
		static bool StartUploadOnlyTask(
			const TArray<FString>& InZipFilePaths,
			const FString& InPackagedPluginsPath,
			const FString& InPluginName,
			bool bInGetShareUrls = true
		);

		// Returns whether package processing is being done.
		static bool IsPackagePluginTaskRunning();

		// Releases all static state. Must be called before Slate is torn down (e.g., from ShutdownModule).
		static void CleanupStatics();
		
		// FTickableObjectBase interface.
		virtual void Tick(float DeltaTime) override;
		virtual bool IsTickable() const override;
		virtual TStatId GetStatId() const override;
		virtual bool IsTickableWhenPaused() const override;
		virtual bool IsTickableInEditor() const override;
		// End of FTickableObjectBase interface.

	private:
		// Called once when package processing begins.
		void Initialize();

		// Called once when package processing ends.
		void Terminate();

		// Opens and focus the Output Log tab.
		static void OpenOutputLog();

		// Called when the editor notification cancel button is pressed.
		void OnCancelButtonPressed();

		// Builds a notification text string reflecting the given task's fine-grained progress.
		FText BuildNotificationText(const TSharedRef<IPluginBuilderTask>& CurrentTask) const;
		
	private:
		// The running task that packages a plugin.
		static TUniquePtr<FPluginPackager> Instance;

		// The editor notification item that package a plugin.
		static FEditorNotificationHandle PendingNotificationHandle;

		// The dataset used to process plugin packages.
		FPackagePluginParams Params;

		// The list of tasks scheduled to process.
		TArray<TSharedRef<IPluginBuilderTask>> Tasks;

		// Zip tasks kept alive so FUploadToCloudTask can read their output paths.
		TArray<TSharedPtr<FZipUpPluginTask>> ZipTaskRefs;
		
		// The total number of tasks scheduled when packaging begins, and per-type totals.
		int32 TotalTaskCount = 0;
		int32 TotalBuildCount = 0;
		int32 TotalZipCount = 0;
		int32 TotalUploadCount = 0;

		// Whether the task was canceled.
		bool bWasCanceled = false;

		// Whether any error occurred during the packaging process.
		bool bHasAnyError = false;

		// Whether this instance was started in upload-only mode (no build/zip tasks).
		bool bIsUploadOnlyMode = false;

		// Elapsed time since the last in-progress notification update.
		float NotificationUpdateTimer = 0.f;

		// How often (in seconds) to refresh the notification text while a task is processing.
		static constexpr float NotificationUpdateInterval = 0.25f;
	};
}
