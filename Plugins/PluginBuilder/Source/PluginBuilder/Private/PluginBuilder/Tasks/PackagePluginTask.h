// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"

namespace PluginBuilder
{
	/**
	 * A dataset used to process plugin packages.
	 */
	struct FPackagePluginArgs
	{
	public:
		// The name of the plugin to build.
		FString PluginName;
		
		// The version name of the plugin to build.
		FString PluginVersionName;

		// Whether the plugin to build uses the content folder.
		bool bCanContainContent;

		// The path to the .uplugin file for the plugin you want to build.
		FString UPluginFile;
		
		// A list of engine versions to build the plugin.
		TArray<FString> EngineVersions;

		// The path of the directory where pre-built plugins, zipped plugins, etc. are output.
		FString OutputDirectoryPath;
		
		// A list of target platforms to build the plugin.
		TArray<FString> TargetPlatforms;
		
		// Whether to handle older versions that do not use the Rocket.txt file.
		bool bRocket;

		// Whether to create a subfolder in the output built plugins folder.
		bool bCreateSubFolder;

		// Whether to judge the header inclusion of the plug-in code strictly.
		bool bStrictIncludes;

		// Whether to create a zip file that contains only the files we need after the build.
		bool bZipUp;

	public:
		// Constructor.
		// If you use the default constructor, set each value from the editor preferences.
		FPackagePluginArgs();
		FPackagePluginArgs(
			const FString& InPluginName,
			const FString& InPluginVersionName,
			const bool bInCanContainContent,
			const FString& InUPluginFile,
			const TArray<FString>& InEngineVersions,
			const FString& InOutputDirectoryPath,
			const TArray<FString>& InTargetPlatforms = TArray<FString>{},
			const bool bInRocket = true,
			const bool bInCreateSubFolder = false,
			const bool bInStrictIncludes = false,
			const bool bInZipUp = true
		);
	};
	
	/**
	 * A task class that packages plugins.
	 */
	class PLUGINBUILDER_API FPackagePluginTask : public FTickableGameObject
	{
	public:
		// Define an event that will be called when the task ends.
		DECLARE_DELEGATE_TwoParams(FOnTaskFinished, const bool /* bWasSuccessful */, const bool /* bWasCanceled */);
		
	public:
		// Create a task to specify a dataset and package the plug-in.
		static TSharedRef<FPackagePluginTask> CreateTask(const FPackagePluginArgs& InArgs = FPackagePluginArgs());

		// Destructor.
		virtual ~FPackagePluginTask() override;

		// FTickableObjectBase interface.
		virtual TStatId GetStatId() const override;
		virtual bool IsTickable() const override;
		virtual void Tick(float DeltaTime) override;
		// End of FTickableObjectBase interface.

		// FTickableGameObject interface.
		virtual bool IsTickableWhenPaused() const override;
		virtual bool IsTickableInEditor() const override;
		// End of FTickableGameObject interface.
		
		// Returns the dataset set for this task.
		const FPackagePluginArgs& GetArgs() const;
		
		// Start the plugin package process.
		void StartProcess(const FOnTaskFinished& Callback);

		// Request cancel the plugin package process.
		// Wait for the build process that is running at the time of request to complete.
		void RequestCancelProcess();
		
	private:
		//
		FString GetDestinationDirectoryName() const;
		FString GetBuiltPluginDestinationPath() const;
		FString GetPackagedPluginDestinationPath() const;
		FString GetZipTempDirectoryPath() const;
		
	private:
		// A dataset used to process plugin packages.
		FPackagePluginArgs Args;

		// An event called when the task ends.
		FOnTaskFinished OnTaskFinished;

		// Whether task processing has started.
		bool bStartedTask = false;
		
		// Whether the task needs to be canceled.
		bool bNeedToCancel = false;

		//
		bool bHasAnyError = false;

		//
		FProcHandle ProcessHandle;
		
		//
		void* ReadPipe = nullptr;

		//
		int32 ProcessingIndex = INDEX_NONE;
	};
}
