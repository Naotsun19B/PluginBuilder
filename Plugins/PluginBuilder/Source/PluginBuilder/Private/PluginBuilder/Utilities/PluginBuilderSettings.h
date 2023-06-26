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
	
	// The path to the directory where the packaged plugin will be output.
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

	// The name of the currently selected plugin to build.
	// This item saves a value for each project.
	UPROPERTY(Config)
	TMap<FName, FName> SelectedBuildTargetNamePerProject;
	TOptional<PluginBuilder::FBuildTargets::FBuildTarget> SelectedBuildTarget;
	
	// The list of engine versions to build the plugin.
	UPROPERTY(Config)
	TArray<FString> EngineVersions;

	// The list of target platforms to build the plugin.
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

	// The compressibility strength. Specify from 0 to 9.
	UPROPERTY(Config)
	uint8 CompressionLevel;
	
public:
	// Constructor.
	UPluginBuilderSettings();
	
	// Registers-Unregisters in the editor setting item.
	static void Register();
	static void Unregister();
	
	// Returns reference of this settings.
	static const UPluginBuilderSettings& Get();

	// Opens the settings menu for this plugin.
	static void OpenSettings();

	// An enum class that defines post modified processing.
	enum class EPostModifiedProcessing : uint8
	{
		None,
		SortEngineVersion,
	};
	
	// Modifies to the properties of the default object of this class.
	static void ModifyProperties(
		const TFunction<void(UPluginBuilderSettings& Settings)>& Predicate,
		const EPostModifiedProcessing PostModifiedProcessing = EPostModifiedProcessing::None
	);
	
	// UObject interface.
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	// End of UObject interface.

	// Resets the output directory path to its default value (Build folder directly under the project folder).
	void ResetOutputDirectoryPath();

	// Returns the name of the build target associated with the project.
	bool GetSelectedBuildTargetName(FName& SelectedBuildTargetName) const;

	// Sets the name of the build target associated with the project.
	void SetSelectedBuildTargetName(const FName& SelectedBuildTargetName);
	
	// Returns whether the parameters is ready to start package plugin task.
	bool IsReadyToStartPackagePluginTask() const;
};
