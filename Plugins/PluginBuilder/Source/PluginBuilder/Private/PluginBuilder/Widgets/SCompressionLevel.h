// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "ToolMenuEntry.h"

namespace PluginBuilder
{
	/**
	 * A spinbox widget class that specifies the compression level of the plugin's zip file.
	 */
	class PLUGINBUILDER_API SCompressionLevel : public SNumericEntryBox<uint8>
	{
	public:
		SLATE_BEGIN_ARGS(SCompressionLevel)
		{}
		SLATE_END_ARGS()

		// Constructor.
		void Construct(const FArguments& InArgs);

		// Generates and returns a widget to display on the tools menu.
		static FToolMenuEntry MakeToolMenuWidget();
	};
}

