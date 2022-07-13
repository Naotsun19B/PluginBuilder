// Copyright 2022 Naotsun. All Rights Reserved.

#include "PluginBuilder/CommandActions/PluginBuilderCommands.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommandActions.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "Interfaces/IMainFrameModule.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "PluginBuilderCommands"

namespace PluginBuilder
{
	FPluginBuilderCommands::FPluginBuilderCommands()
		: TCommands<FPluginBuilderCommands>
		(
			TEXT("PluginBuilder"),
			NSLOCTEXT("Contexts", "PluginBuilder", "Plugin Builder"),
			NAME_None,
			FEditorStyle::Get().GetStyleSetName()
		)
		, CommandBindings(MakeShared<FUICommandList>())
		, bIsBound(false)
	{
	}

	void FPluginBuilderCommands::RegisterCommands()
	{
		// Register command here.
		UI_COMMAND(
			BuildPlugin,
			"Build Plugin",
			"Builds and packages the plugin based on the set build configuration.",
			EUserInterfaceActionType::Button,
			FInputChord()
		);

		UI_COMMAND(
			OpenBuildSettings,
			"Build Settings...",
			"Opens the settings for Plugin Builder.",
			EUserInterfaceActionType::Button,
			FInputChord()
		);
	}

	bool FPluginBuilderCommands::IsBound()
	{
		check(Instance.IsValid());
		return Instance.Pin()->bIsBound;
	}

	void FPluginBuilderCommands::Bind()
	{
		check(Instance.Pin().IsValid());
		Instance.Pin()->BindCommands();
	}

	void FPluginBuilderCommands::BindCommands()
	{
		if (!IsRegistered())
		{
			UE_LOG(LogPluginBuilder, Fatal, TEXT("Bound before UI Command was registered.\nPlease be sure to bind after registration."));
		}

		if (IsBound())
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("The binding process has already been completed."));
			return;
		}
		bIsBound = true;
		
		const TSharedRef<FUICommandList>& MainFrameCommandBindings = IMainFrameModule::Get().GetMainFrameCommandBindings();
		MainFrameCommandBindings->Append(CommandBindings);
		
		CommandBindings->MapAction(
			BuildPlugin,
			FExecuteAction::CreateStatic(&FPluginBuilderCommandActions::BuildPlugin),
			FCanExecuteAction::CreateStatic(&FPluginBuilderCommandActions::CanBuildPlugin)
		);

		CommandBindings->MapAction(
			OpenBuildSettings,
			FExecuteAction::CreateStatic(&FPluginBuilderCommandActions::OpenBuildSettings)
		);
	}
}

#undef LOCTEXT_NAMESPACE
