// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * Information on the plugin to be built.
	 */
	class PLUGINBUILDER_API FBuildTarget
	{
	public:
		// Returns a filtered list of plugins based on the settings to be built.
		static TArray<FBuildTarget> GetFilteredBuildTargets();
		
		// Returns the default build target inferred from the project name.
		static TOptional<FBuildTarget> GetDefaultBuildTarget();

		// Select the specified build target.
		static void SelectBuildTarget(const FBuildTarget BuildTarget);

		// Returns whether the specified build target is selected.
		static bool IsSelectedBuildTarget(const FBuildTarget BuildTarget);
		
		// Constructor.
		FBuildTarget(const FString& InPluginName, const FString InPluginDescription);

		// Returns the name of the plugin to build.
		FText GetPluginName() const;

		// Returns the description of the plugin to build.
		FText GetPluginDescription() const;

		// Returns the icon of the plugin to build.
		FSlateIcon GetPluginIcon() const;
	
	private:
		// The name of the plugin to build.
		FString PluginName;

		// The description of the plugin to build.
		FString PluginDescription;
	};
}
