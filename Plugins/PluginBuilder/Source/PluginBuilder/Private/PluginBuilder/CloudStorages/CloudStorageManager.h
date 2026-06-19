// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	class ICloudStorageProvider;

	/**
	 * Manages the active cloud storage provider.
	 * Use GetCurrentProvider() to obtain the provider selected in editor preferences.
	 * Additional providers can be registered to support future cloud storage services.
	 */
	class PLUGINBUILDER_API FCloudStorageManager
	{
	public:
		// Returns the currently active cloud storage provider, or nullptr if none is configured.
		static TSharedPtr<ICloudStorageProvider> GetCurrentProvider();

		// Registers a custom provider, replacing the current one.
		static void RegisterProvider(TSharedPtr<ICloudStorageProvider> InProvider);

	private:
		// The currently active cloud storage provider instance.
		static TSharedPtr<ICloudStorageProvider> CurrentProvider;
	};
}
