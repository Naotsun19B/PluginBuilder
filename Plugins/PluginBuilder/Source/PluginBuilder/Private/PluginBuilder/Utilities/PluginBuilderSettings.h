// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "PluginBuilder/Types/BuildTargets.h"
#if UE_5_00_OR_LATER
#include "UObject/SoftObjectPath.h"
#else
#include "Engine/EngineTypes.h"
#endif
#include "PluginBuilderSettings.generated.h"

/**
 * A editor preferences class for this plugin.
 */
UCLASS(Config = Editor, GlobalUserConfig)
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

	// Whether to use the Build directory for each project as the output directory.
	UPROPERTY(EditAnywhere, Config, Category = "Output", meta = (EditCondition = "!bSelectOutputDirectoryManually"))
	bool bOutputToBuildDirectoryOfEachProject;
	
	// A path to the directory where the packaged plugin will be output.
	UPROPERTY(EditAnywhere, Config, Category = "Output", meta = (EditCondition = "!bSelectOutputDirectoryManually"))
	FDirectoryPath OutputDirectoryPath;

	// Whether to use friendly names in plugin output files, editor menus, etc.
	UPROPERTY(EditAnywhere, Config, Category = "Misc")
	bool bUseFriendlyName;

	// Whether to change the output log filter to show only log categories for this plugin when starting the package process.
	// This feature is only available in UE5.1 or later.
	UPROPERTY(EditAnywhere, Config, Category = "Misc")
	bool bShowOnlyLogsFromThisPluginWhenPackageProcessStarts;

	// Whether to stop the packaging process as soon as the cancel button is pressed during packaging.
	// !!CAUTION!! You may get an error from UBT as it kills the process.
	UPROPERTY(EditAnywhere, Config, Category = "Misc")
	bool bStopPackagingProcessImmediately;

	// A name of the currently selected plugin to build.
	UPROPERTY(Config)
	FName SelectedBuildTargetName;
	TOptional<PluginBuilder::FBuildTargets::FBuildTarget> SelectedBuildTarget;
	
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

	// Whether to put the zip files into a single folder.
	// If false will use a per engine folder for each zip file.
	UPROPERTY(Config)
	bool bOutputAllZipFilesToSingleFolder;
	
	// Whether the zip folder should keep the binaries folder.
	// Marketplace submissions expect the binaries folder to be deleted.
	UPROPERTY(Config)
	bool bKeepBinariesFolder;

	// Whether to keep the properties of uplugin that are deleted when outputting from UAT even after outputting.
	// Examples of properties to delete by UAT: IsBetaVersion, IsExperimentalVersion, EnabledByDefault, etc.
	// Marketplace submissions expect to use the uplugin file output by UAT.
	UPROPERTY(Config)
	bool bKeepUPluginProperties;
	
public:
	// Constructor.
	UPluginBuilderSettings();
	
	// Registers-Unregisters in the editor setting item.
	static void Register();
	static void Unregister();
	
	// Returns reference of this settings.
	static UPluginBuilderSettings& Get();

	// Opens the settings menu for this plugin.
	static void OpenSettings();

	// UObject interface.
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	// End of UObject interface.

	// Resets the output directory path to its default value (Build folder directly under the project folder).
	void ResetOutputDirectoryPath();
	
	// Returns whether the parameters is ready to start package plugin task.
	bool IsReadyToStartPackagePluginTask() const;
};
