// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * A class that manages the name of the platform to be built.
	 */
	class PLUGINBUILDER_API FPlatformsBase
	{
	public:
		// A structure that summarizes platform names in UBT and ini formats respectively.
		struct PLUGINBUILDER_API FPlatform
		{
		public:
			// The name of the platform covered by UBT.
			FString UBTPlatformName;

			// The name of the platform covered by ini.
			FString IniPlatformName;

			// The identifier to group similar platforms together, such as "Mobile" and "Console".
			FName PlatformGroupName;

			// The style name of the icon linked to the platform.
			// If there is not enough SDK or something and is not available, an error icon.
			FName IconStyleName;

			// Whether the required SDK is installed and builds for this platform is possible.
			bool bIsAvailable = false;
		};

	protected:
		// Define the event that filters available platforms when collecting.
		DECLARE_DELEGATE_RetVal_OneParam(bool, FFilterPlatform, const ITargetPlatform& /* Platform */);
		
	protected:
		// Constructor.
		explicit FPlatformsBase(const FFilterPlatform& InFilterPlatform);
		
		// Returns a list of available platform names.
		TArray<FPlatform> GetPlatformNames(const bool bWithRefresh);

		// Collects available platform names from ITargetPlatformManagerModule.
		void RefreshPlatformNames();

		// Returns whether an SDK that requires a platform with the specified name is installed and can be built for this platform.
		bool IsAvailablePlatform(const FString& PlatformName) const;

	private:
		// The list of available platforms.
		TArray<FPlatform> Platforms;
		
		// The filter used when collecting available platforms.
		FFilterPlatform FilterPlatform;
	};
}
