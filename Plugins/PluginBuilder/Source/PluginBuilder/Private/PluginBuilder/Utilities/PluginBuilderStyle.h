// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

namespace PluginBuilder
{
	/**
	 * A class that manages the slate icon used by this plugin.
	 */
	class PLUGINBUILDER_API FPluginBuilderStyle : public FSlateStyleSet
	{
	public:
		// Constructor.
		FPluginBuilderStyle();

	private:
		// The actual registration process for this class.
		void RegisterInternal();
		
	public:
		// Registers-Unregisters and instance getter this class.
		static void Register();
		static void Unregister();
		static const ISlateStyle& Get();

		// Gets the registered name of the icon from the plugin name.
		static FName GetPropertyName(const FString& PluginFriendlyName);
		
	private:
		// The instance of this style class.
		static TSharedPtr<FPluginBuilderStyle> Instance;
	};
}
