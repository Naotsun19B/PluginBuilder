// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PluginBuilder/Types/BuildTarget.h"
#include "PluginBuilderSettings.generated.h"

/**
 * Editor settings for this plugin.
 */
UCLASS(Config = Editor)
class PLUGINBUILDER_API UPluginBuilderSettings : public UObject
{
	GENERATED_BODY()

public:
	// Whether to ensure that only valid plugins appear in the list of build targets.
	UPROPERTY(EditAnywhere, Config, Category = "Build Target")
	bool bSearchOnlyEnabled;

	// Whether to make plugins in the project's plugins folder appear in the list of build targets.
	UPROPERTY(EditAnywhere, Config, Category = "Build Target")
	bool bContainsProjectPlugins;

	// Whether to make plugins in the engine's plugins folder appear in the list of build targets.
	UPROPERTY(EditAnywhere, Config, Category = "Build Target")
	bool bContainsEnginePlugins;

	// Whether to select the output directory before starting the packaging process.
	UPROPERTY(EditAnywhere, Config, Category = "Output")
	bool bSelectOutputDirectoryManually;
	
	// The path to the directory where the packaged plugin will be output.
	UPROPERTY(EditAnywhere, Config, Category = "Output", meta = (EditCondition = "!bSelectOutputDirectoryManually"))
	FDirectoryPath OutputDirectoryPath;

	// Whether to stop the packaging process as soon as the cancel button is pressed during packaging.
	// !!CAUTION!! You may get an error from UBT as it kills the process.
	UPROPERTY(EditAnywhere, Config, Category = "Process")
	bool bStopPackagingProcessImmediately;

	// The name of the currently selected plugin to build.
	TOptional<PluginBuilder::FBuildTarget> SelectedBuildTarget;
	
	// A list of engine versions to build the plugin.
	UPROPERTY(Config)
	TArray<FString> EngineVersions;

	// A list of target platforms to build the plugin.
	UPROPERTY(Config)
	TArray<FString> TargetPlatforms;

	// Whether to handle older versions that do not use the Rocket.txt file.
	UPROPERTY(Config)
	bool bRocket;

	// Whether to create a subfolder in the output built plugins folder.
	UPROPERTY(Config)
	bool bCreateSubFolder;

	// Whether to judge the header inclusion of the plug-in code strictly.
	UPROPERTY(Config)
	bool bStrictIncludes;

	// Whether to create a zip file that contains only the files we need after the build.
	UPROPERTY(Config)
	bool bZipUp;
	
public:
	// Constructor.
	UPluginBuilderSettings();
	
	// Register - unregister in the editor setting item.
	static void Register();
	static void Unregister();
	
	// Returns reference of this settings.
	static UPluginBuilderSettings& Get();

	// Open the settings menu for this plugin.
	static void OpenSettings();
};
