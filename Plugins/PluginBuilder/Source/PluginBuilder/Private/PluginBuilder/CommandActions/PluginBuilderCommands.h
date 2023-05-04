// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/UICommandInfo.h"

namespace PluginBuilder
{
	/**
	 * Class to register the shortcut key used in this plugin.
	 */
	class PLUGINBUILDER_API FPluginBuilderCommands : public TCommands<FPluginBuilderCommands>
	{
	public:
		// Constructor.
		FPluginBuilderCommands();

		// TCommands interface.
		virtual void RegisterCommands() override;
		// End of TCommands interface.

		// Is the command registered here bound.
		static bool IsBound();

		// Processing of command binding.
		static void Bind();
	
	protected:
		// Internal processing of command binding.
		virtual void BindCommands();

	public:
		// A list of shortcut keys used by this plugin.
		TSharedRef<FUICommandList> CommandBindings;
		
		// Instances of bound commands.
		TSharedPtr<FUICommandInfo> BuildPlugin;
		TSharedPtr<FUICommandInfo> Rocket;
		TSharedPtr<FUICommandInfo> CreateSubFolder;
		TSharedPtr<FUICommandInfo> StrictIncludes;
		TSharedPtr<FUICommandInfo> ZipUp;
		TSharedPtr<FUICommandInfo> OpenBuildSettings;

	private:
		// Is the command registered here bound.
		bool bIsBound;
	};
}
