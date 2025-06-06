// Copyright 2022-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/Types/PlatformsBase.h"

namespace PluginBuilder
{
	/**
	 * A class that manages the name of the target platform to be built.
	 */
	class PLUGINBUILDER_API FTargetPlatforms : public FPlatformsBase
	{
	public:
		// Constructor.
		FTargetPlatforms();
		
		// Returns a list of available target platform names.
		static TArray<FPlatform> GetTargetPlatformNames(const bool bWithRefresh = true);

		// Collects available target platform names from ITargetPlatformManagerModule.
		static void RefreshTargetPlatformNames();

		// Returns whether an SDK that requires a platform with the specified name is installed and can be built for this platform.
		static bool IsAvailableTargetPlatform(const FString& TargetPlatformName);

		// Logs the available host platforms.
		static void LogAvailableTargetPlatformNames();
		
		// Toggles selection state the specified target platform.
		static void ToggleTargetPlatform(const FPlatform TargetPlatform);

		// Returns whether the specified target platform is selected.
		static bool GetTargetPlatformState(const FPlatform TargetPlatform);
	};
}
