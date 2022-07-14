// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	class FPackagePluginTask;
	
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

	private:
		// Called when the cancel button of the editor notification is pressed.
		static void HandleOnCancelPackagingButtonPressed();
		
		// Called when the plugin packaging process is complete.
		static void HandleOnTaskFinished(
			const bool bWasSuccessful,
			const bool bWasCanceled,
			TSharedPtr<SNotificationItem> PendingNotificationItem
		);
		
	private:
		// A running task that packages a plugin.
		static TSharedPtr<FPackagePluginTask> ActivePackagePluginTask;
	};
}
