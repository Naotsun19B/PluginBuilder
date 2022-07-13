// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * A class that defines the function executed from the shortcut key.
	 */
	class PLUGINBUILDER_API FPluginBuilderCommandActions
	{
	public:
		// Builds and packages the plugin based on the set build configuration.
		static void BuildPlugin();
		static bool CanBuildPlugin();
		
		// Opens the settings for Plugin Builder.
		static void OpenBuildSettings();
	};
}
