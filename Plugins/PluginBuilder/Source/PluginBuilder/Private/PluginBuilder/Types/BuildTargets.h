// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Textures/SlateIcon.h"

class IPlugin;

namespace PluginBuilder
{
	/**
	 * A class that obtains the list of build target plugin information from the plugin manager.
	 */
	class PLUGINBUILDER_API FBuildTargets
	{
	public:
		// Information about the plugin being built obtained from the plugin manager.
		struct PLUGINBUILDER_API FBuildTarget
		{
		public:
			// Constructor.
			explicit FBuildTarget(const TSharedRef<IPlugin>& Plugin);

			// Returns the name of the plugin to build.
			FString GetPluginName() const;

			// Returns the description of the plugin to build.
			FString GetPluginDescription() const;

			// Returns the name of the category of the plugin to build.
			FString GetPluginCategory() const;

			// Returns the icon of the plugin to build.
			FSlateIcon GetPluginIcon() const;

			// Returns the version name of the plugin to build.
			FString GetPluginVersionName() const;

			// Returns whether the plugin to build uses the content folder.
			bool CanPluginContainContent() const;

			// Returns the path to the .uplugin file for the plugin you want to build.
			FString GetUPluginFile() const;
		
		private:
			// The name of the plugin to build.
			FString PluginName;

			// The description of the plugin to build.
			FString PluginDescription;

			// The name of the category of the plugin to build.
			FString PluginCategory;
		
			// The version name of the plugin to build.
			FString PluginVersionName;

			// Whether the plugin to build uses the content folder.
			bool bCanPluginContainContent;

			// The path to the .uplugin file for the plugin you want to build.
			FString UPluginFile;
		};
		
	public:
		// Returns a filtered list of plugins based on the settings to be built.
		static TArray<FBuildTarget> GetFilteredBuildTargets();
		
		// Returns the default build target inferred from the project name.
		static TOptional<FBuildTarget> GetDefaultBuildTarget();

		// Selects the specified build target.
		static void SelectBuildTarget(const FBuildTarget BuildTarget);

		// Returns whether the specified build target is selected.
		static bool IsBuildTargetSelected(const FBuildTarget BuildTarget);
	};
}
