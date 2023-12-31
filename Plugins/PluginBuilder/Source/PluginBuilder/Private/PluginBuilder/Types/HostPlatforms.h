// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/Types/PlatformsBase.h"

namespace PluginBuilder
{
	/**
	 * A class that manages the name of the host platform to be built.
	 */
	class PLUGINBUILDER_API FHostPlatforms : public FPlatformsBase
	{
	public:
		// Constructor.
		FHostPlatforms();
		
		// Returns a list of available host platform names.
		static TArray<FPlatform> GetHostPlatformNames(const bool bWithRefresh = true);

		// Collects available host platform names from ITargetPlatformManagerModule.
		static void RefreshHostPlatformNames();
		
		// Toggles selection state the specified host platform.
		static void ToggleHostPlatform(const FPlatform HostPlatform);

		// Returns whether the specified host platform is selected.
		static bool GetHostPlatformState(const FPlatform HostPlatform);
		
		// Toggles whether to prevent editor platform compilation on the host.
		static void ToggleNoHostPlatform();

		// Returns whether to prevent editor platform compilation on the host.
		static bool GetNoHostPlatformState();
	};
}
