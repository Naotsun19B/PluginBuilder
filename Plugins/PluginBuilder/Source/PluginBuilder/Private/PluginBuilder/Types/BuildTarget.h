// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IPlugin;

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
		explicit FBuildTarget(const TSharedRef<IPlugin>& Plugin);

		// Returns the name of the plugin to build.
		FString GetPluginName() const;

		// Returns the description of the plugin to build.
		FString GetPluginDescription() const;

		// Returns the icon of the plugin to build.
		FSlateIcon GetPluginIcon() const;

		// Returns the version name of the plugin to build.
		FString GetPluginVersionName() const;

		// Returns whether the plugin to build uses the content folder.
		bool CanContainContent() const;

		// Returns the path to the .uplugin file for the plugin you want to build.
		FString GetUPluginFile() const;
		
	private:
		// The name of the plugin to build.
		FString PluginName;

		// The description of the plugin to build.
		FString PluginDescription;

		// The version name of the plugin to build.
		FString PluginVersionName;

		// Whether the plugin to build uses the content folder.
		bool bCanContainContent;

		// The path to the .uplugin file for the plugin you want to build.
		FString UPluginFile;
	};
}
