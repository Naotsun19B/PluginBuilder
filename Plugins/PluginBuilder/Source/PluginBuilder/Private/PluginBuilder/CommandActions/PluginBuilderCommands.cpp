// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/CommandActions/PluginBuilderCommands.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommandActions.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "Interfaces/IMainFrameModule.h"
#if UE_5_00_OR_LATER
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif

#define LOCTEXT_NAMESPACE "PluginBuilderCommands"

namespace PluginBuilder
{
	FPluginBuilderCommands::FPluginBuilderCommands()
		: TCommands<FPluginBuilderCommands>
		(
			Global::PluginName,
			LOCTEXT("Contexts", "Plugin Builder"),
			NAME_None,
#if UE_5_00_OR_LATER
			FAppStyle::GetAppStyleSetName()
#else
			FEditorStyle::GetStyleSetName()
#endif
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
			Rocket,
			"Rocket",
			"Whether to handle older versions that do not use the Rocket.txt file.",
			EUserInterfaceActionType::RadioButton,
			FInputChord()
		);

		UI_COMMAND(
			CreateSubFolder,
			"Create Sub Folder",
			"Whether to create a subfolder in the output built plugins folder.",
			EUserInterfaceActionType::RadioButton,
			FInputChord()
		);

		UI_COMMAND(
			StrictIncludes,
			"Strict Includes",
			"Whether to judge the header inclusion of the plugin code strictly.",
			EUserInterfaceActionType::RadioButton,
			FInputChord()
		);

		UI_COMMAND(
			ZipUp,
			"Zip Up",
			"Whether to create a zip file that contains only the files we need after the build.",
			EUserInterfaceActionType::RadioButton,
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
			Rocket,
			FExecuteAction::CreateStatic(&FPluginBuilderCommandActions::ToggleRocket),
			FCanExecuteAction(),
			FIsActionChecked::CreateStatic(&FPluginBuilderCommandActions::GetRocketState)
		);

		CommandBindings->MapAction(
			CreateSubFolder,
			FExecuteAction::CreateStatic(&FPluginBuilderCommandActions::ToggleCreateSubFolder),
			FCanExecuteAction(),
			FIsActionChecked::CreateStatic(&FPluginBuilderCommandActions::GetCreateSubFolderState)
		);

		CommandBindings->MapAction(
			StrictIncludes,
			FExecuteAction::CreateStatic(&FPluginBuilderCommandActions::ToggleStrictIncludes),
			FCanExecuteAction(),
			FIsActionChecked::CreateStatic(&FPluginBuilderCommandActions::GetStrictIncludesState)
		);

		CommandBindings->MapAction(
			ZipUp,
			FExecuteAction::CreateStatic(&FPluginBuilderCommandActions::ToggleZipUp),
			FCanExecuteAction(),
			FIsActionChecked::CreateStatic(&FPluginBuilderCommandActions::GetZipUpState)
		);

		CommandBindings->MapAction(
			OpenBuildSettings,
			FExecuteAction::CreateStatic(&FPluginBuilderCommandActions::OpenBuildSettings)
		);
	}
}

#undef LOCTEXT_NAMESPACE
