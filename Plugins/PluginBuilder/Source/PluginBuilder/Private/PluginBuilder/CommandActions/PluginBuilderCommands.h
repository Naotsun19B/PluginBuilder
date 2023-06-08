// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/UICommandInfo.h"

namespace PluginBuilder
{
	/**
	 * A class to register the shortcut key used in this plugin.
	 */
	class PLUGINBUILDER_API FPluginBuilderCommands : public TCommands<FPluginBuilderCommands>
	{
	public:
		// Constructor.
		FPluginBuilderCommands();

		// TCommands interface.
		virtual void RegisterCommands() override;
		// End of TCommands interface.

		// Returns whether the commands registered here are bound.
		static bool IsBound();

		// Binds commands registered here.
		static void Bind();
	
	protected:
		// Binds commands actually registered here.
		virtual void BindCommands();

	public:
		// The list of shortcut keys used by this plugin.
		TSharedRef<FUICommandList> CommandBindings;
		
		// Instances of bound commands.
		TSharedPtr<FUICommandInfo> BuildPlugin;
		TSharedPtr<FUICommandInfo> Rocket;
		TSharedPtr<FUICommandInfo> CreateSubFolder;
		TSharedPtr<FUICommandInfo> StrictIncludes;
		TSharedPtr<FUICommandInfo> ZipUp;
		TSharedPtr<FUICommandInfo> KeepBinariesFolder;
		TSharedPtr<FUICommandInfo> OutputAllZipFilesToSingleFolder;
		TSharedPtr<FUICommandInfo> KeepUPluginProperties;
		TSharedPtr<FUICommandInfo> OpenBuildSettings;

	private:
		// Whether the commands registered here are bound.
		bool bIsBound;
	};
}
