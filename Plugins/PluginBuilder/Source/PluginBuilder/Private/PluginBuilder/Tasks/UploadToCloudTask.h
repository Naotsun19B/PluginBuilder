// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/Tasks/IPluginBuilderTask.h"

namespace PluginBuilder
{
	class ICloudStorageProvider;
	class FZipUpPluginTask;

	/**
	 * A task that uploads completed zip files to a cloud storage provider
	 * and optionally retrieves an edit-permission share URL for each file.
	 * Results are logged to the Output Log and, when share URLs are requested,
	 * saved to a text file under Saved/PluginBuilder/.
	 */
	class PLUGINBUILDER_API FUploadToCloudTask : public IPluginBuilderTask
	{
	public:
		// Constructor for auto-upload: receives references to completed zip tasks.
		FUploadToCloudTask(
			const TArray<TSharedPtr<FZipUpPluginTask>>& InZipTasks,
			const FString& InPackagedPluginsPath,
			const FString& InPluginName,
			bool bInGetShareUrls
		);

		// Constructor for manual upload: receives an explicit list of local zip file paths.
		FUploadToCloudTask(
			const TArray<FString>& InZipFilePaths,
			const FString& InPackagedPluginsPath,
			const FString& InPluginName,
			bool bInGetShareUrls
		);

		// IPluginBuilderTask interface.
		virtual EState GetState() const override;
		virtual bool HasAnyError() const override;
		virtual FString GetTaskLabel() const override;
		virtual void Initialize() override;
		virtual void Tick(float DeltaTime) override;
		virtual void Terminate() override;
		// End of IPluginBuilderTask interface.

	private:
		// Starts uploading the next pending file.
		void ProcessNextFile();

		// Finishes processing: logs all results and optionally writes share URLs to disk.
		void FinalizeResults();

		// Builds the remote file path for a given local zip file.
		FString BuildRemotePath(const FString& LocalZipFilePath) const;

		// Writes the collected share URLs to Saved/PluginBuilder/ShareUrls_<timestamp>.txt.
		void WriteShareUrlsToFile() const;

	private:
		// References to completed zip tasks (used to read their zip file paths).
		TArray<TSharedPtr<FZipUpPluginTask>> ZipTasks;

		// Resolved local file paths to upload.
		TArray<FString> ZipFilePaths;

		// The PackagedPlugins directory path used to compute relative remote paths.
		FString PackagedPluginsPath;

		// The plugin name, used as the top-level folder in the remote storage.
		FString PluginName;

		// Whether to retrieve a share URL for each uploaded file.
		bool bGetShareUrls;

		// Current task state.
		EState State;

		// Whether any file upload or URL retrieval failed.
		bool bHasAnyError;

		// Whether an HTTP request is currently in flight.
		bool bHttpRequestPending;

		// Index of the file currently being processed.
		int32 CurrentFileIndex;

		// Tracks local paths of files that were uploaded successfully.
		TSet<FString> SuccessfulUploads;

		// Share URLs keyed by local zip path. Only populated when bGetShareUrls is true.
		TMap<FString, FString> ShareUrlResults;

		// The cloud storage provider used for this task.
		TSharedPtr<ICloudStorageProvider> Provider;
	};
}
