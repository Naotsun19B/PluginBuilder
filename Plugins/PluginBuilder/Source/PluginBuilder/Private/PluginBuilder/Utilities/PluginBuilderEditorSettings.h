// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#if UE_5_00_OR_LATER
#include "UObject/SoftObjectPath.h"
#else
#include "Engine/EngineTypes.h"
#endif
#include "PluginBuilderEditorSettings.generated.h"

/**
 * An editor preferences class for features in the editor.
 */
UCLASS(GlobalUserConfig)
class PLUGINBUILDER_API UPluginBuilderEditorSettings : public UPluginBuilderSettings
{
	GENERATED_BODY()

public:
	// This class is read-only.
	using UReference = const UPluginBuilderEditorSettings&;

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

public:
	// Constructor.
	UPluginBuilderEditorSettings();
	
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
