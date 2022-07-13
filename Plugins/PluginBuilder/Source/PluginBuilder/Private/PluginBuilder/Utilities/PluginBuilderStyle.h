// Copyright 2021-2022 Naotsun. All Rights Reserved.

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
		
		// Register-Unregister and instance getter this class.
		static void Register();
		static void Unregister();
		static const ISlateStyle& Get();

		// Get the registered name of the icon from the plug-in name.
		static FName GetPropertyName(const FString& PluginName);
		
	private:
		// An instance of this style class.
		static TSharedPtr<FPluginBuilderStyle> Instance;
	};
}
