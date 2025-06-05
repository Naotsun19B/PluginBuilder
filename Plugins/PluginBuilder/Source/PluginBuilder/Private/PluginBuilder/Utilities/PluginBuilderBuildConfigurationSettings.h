// Copyright 2022-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/Types/BuildTargets.h"
#include "PluginBuilderBuildConfigurationSettings.generated.h"

/**
 * An editor preferences class for features in the editor.
 */
UCLASS(ProjectUserConfig)
class PLUGINBUILDER_API UPluginBuilderBuildConfigurationSettings : public UPluginBuilderSettings
{
	GENERATED_BODY()

public:
	// This class can also be written.
	using UReference = UPluginBuilderBuildConfigurationSettings&;

public:
	// The name of the currently selected plugin to build.
	// This item saves a value for each project.
	UPROPERTY(VisibleAnywhere, Config, Category = "Project")
	FString SelectedBuildTargetName;
	TOptional<PluginBuilder::FBuildTargets::FBuildTarget> SelectedBuildTarget;
	
	// The list of engine versions to build the plugin.
	UPROPERTY(VisibleAnywhere, Category = "Engine")
	TArray<FString> EngineVersions;
	UPROPERTY(Config)
	FString EngineVersionsString;
	
	// Whether to prevent editor platform compilation on the host.
	UPROPERTY(VisibleAnywhere, Config, Category = "Platform")
	bool bNoHostPlatform;

	// The list of host platforms to build the plugin.
	// If nothing is specified, the current host platform is used.
	UPROPERTY(VisibleAnywhere, Category = "Platform")
	TArray<FString> HostPlatforms;
	UPROPERTY(Config)
	FString HostPlatformsString;
	
	// The list of target platforms to build the plugin.
	// If nothing is specified, all the Rocket target platforms is used.
	UPROPERTY(VisibleAnywhere, Category = "Platform")
	TArray<FString> TargetPlatforms;
	UPROPERTY(Config)
	FString TargetPlatformsString;

	// Whether to handle older versions that do not use the Rocket.txt file.
	UPROPERTY(VisibleAnywhere, Config, Category = "Build")
	bool bRocket;

	// Whether to create a subfolder in the output built plugins folder.
	UPROPERTY(VisibleAnywhere, Config, Category = "Build")
	bool bCreateSubFolder;

	// Whether to judge the header inclusion of the plugin code strictly.
	UPROPERTY(VisibleAnywhere, Config, Category = "Build")
	bool bStrictIncludes;
	
	// Whether to embed the engine version to be built into the uplugin file.
	UPROPERTY(VisibleAnywhere, Config, Category = "Build")
	bool bUnversioned;
	
	// Whether to create a zip file that contains only the files we need after the build.
	UPROPERTY(VisibleAnywhere, Config, Category = "Zip")
	bool bZipUp;

	// Whether to put the zip files into a single folder.
	// If false will use a per engine folder for each zip file.
	UPROPERTY(VisibleAnywhere, Config, Category = "Zip")
	bool bOutputAllZipFilesToSingleFolder;
	
	// Whether the zip folder should keep the binaries folder.
	// Marketplace submissions expect the binaries folder to be deleted.
	UPROPERTY(VisibleAnywhere, Config, Category = "Zip")
	bool bKeepBinariesFolder;

	// Whether to keep the properties of uplugin that are deleted when outputting from UAT even after outputting.
	// Examples of properties to delete by UAT: IsBetaVersion, IsExperimentalVersion, EnabledByDefault, etc.
	// Marketplace submissions expect to use the uplugin file output by UAT.
	UPROPERTY(VisibleAnywhere, Config, Category = "Zip")
	bool bKeepUPluginProperties;

	// The compressibility strength. Specify from 0 to 9.
	UPROPERTY(VisibleAnywhere, Config, Category = "Zip")
	uint8 CompressionLevel;
	
public:
	// Constructor.
	UPluginBuilderBuildConfigurationSettings();
	
	// UObject interface.
	virtual void PostInitProperties() override;
	// End of UObject interface.

	// UPluginBuilderSettings interface.
	virtual FString GetSettingsName() const override;
	virtual void PreSaveConfig() override;
	// End of UPluginBuilderSettings interface.
	
	// Returns whether the parameters is ready to start package plugin task.
	bool IsReadyToStartPackagePluginTask() const;
};
