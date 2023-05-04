// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderMenuExtension.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommands.h"
#include "PluginBuilder/Types/BuildTarget.h"
#include "PluginBuilder/Types/EngineVersions.h"
#include "PluginBuilder/Types/PlatformNames.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "PluginBuilderMenuExtension"

namespace PluginBuilder
{
	const FName FPluginBuilderMenuExtension::FilePluginSectionName		= TEXT("PluginBuilder.FilePlugin");
	const FName FPluginBuilderMenuExtension::PackagePluginSubMenuName	= TEXT("PluginBuilder.PackagePlugin");
	
	void FPluginBuilderMenuExtension::Register()
	{
		UToolMenu* MenuExtensionPoint = GetMenuExtensionPoint();
		if (!IsValid(MenuExtensionPoint))
		{
			return;
		}
		
		FToolMenuSection& FilePluginSection = MenuExtensionPoint->AddSection(
			FilePluginSectionName,
			LOCTEXT("SectionLabel", "Plugin"),
#if UE_5_00_OR_LATER
			FToolMenuInsert()
#else
			FToolMenuInsert(TEXT("FileProject"), EToolMenuInsertType::After)
#endif
		);
		FilePluginSection.AddSubMenu(
			PackagePluginSubMenuName,
			LOCTEXT("PackagePluginLabel", "Package Plugin"),
			LOCTEXT("PackagePluginTooltip", "Build the plugin with multiple engine versions and zip the required files for distribution."),
			FNewToolMenuChoice(FNewMenuDelegate::CreateStatic(&OnExtendPackagePluginSubMenu)),
			false,
			FSlateIcon(
#if UE_5_00_OR_LATER
				FAppStyle::GetAppStyleSetName(),
#else
				FEditorStyle::GetStyleSetName(),
#endif
				TEXT("MainFrame.PackageProject")
			)
		);
	}

	void FPluginBuilderMenuExtension::Unregister()
	{
		UToolMenu* MainFrameFileMenu = GetMenuExtensionPoint();
		if (!IsValid(MainFrameFileMenu))
		{
			return;
		}

		MainFrameFileMenu->RemoveSection(FilePluginSectionName);
	}

	UToolMenu* FPluginBuilderMenuExtension::GetMenuExtensionPoint()
	{
		UToolMenus* ToolMenus = UToolMenus::Get();
		if (!IsValid(ToolMenus))
		{
			return nullptr;
		}

		const FName ExtensionName =
#if UE_5_00_OR_LATER
			TEXT("LevelEditor.MainMenu.Build");
#else
			TEXT("MainFrame.MainTabMenu.File");
#endif

		return ToolMenus->ExtendMenu(ExtensionName);
	}

	void FPluginBuilderMenuExtension::OnExtendPackagePluginSubMenu(FMenuBuilder& MenuBuilder)
	{
		MenuBuilder.AddMenuEntry(FPluginBuilderCommands::Get().BuildPlugin);

		MenuBuilder.AddSeparator();
		
		MenuBuilder.AddSubMenu(
			LOCTEXT("BuildConfigurationLabel", "Build Configurations"),
			LOCTEXT("BuildConfigurationTooltip", "Select the version and platform of the engine to build, whether to zip file, etc."),
			FNewMenuDelegate::CreateStatic(&OnExtendBuildConfigurationSubMenu),
			false,
			{},
			false
		);

		MenuBuilder.AddSubMenu(
			LOCTEXT("BuildTargetLabel", "Build Targets"),
			LOCTEXT("BuildTargetTooltip", "Select the plugin to build."),
			FNewMenuDelegate::CreateStatic(&OnExtendBuildTargetSubMenu),
			false,
			{},
			false
		);
		
		MenuBuilder.AddSeparator();

		MenuBuilder.AddMenuEntry(FPluginBuilderCommands::Get().OpenBuildSettings);
	}

	void FPluginBuilderMenuExtension::OnExtendBuildConfigurationSubMenu(FMenuBuilder& MenuBuilder)
	{
		MenuBuilder.AddSubMenu(
			LOCTEXT("EngineVersionsLabel", "Engine Versions"),
			LOCTEXT("EngineVersionsTooltip", "Specifies the engine version to build the plugin."),
			FNewMenuDelegate::CreateStatic(&OnExtendEngineVersionsSubMenu),
			false,
			{},
			false
		);

		MenuBuilder.AddSubMenu(
			LOCTEXT("TargetPlatformsLabel", "Target Platforms"),
			LOCTEXT("TargetPlatformsTooltip", "Specifies the target platforms to build the plugin."),
			FNewMenuDelegate::CreateStatic(&OnExtendTargetPlatformsSubMenu),
			false,
			{},
			false
		);

		MenuBuilder.AddMenuEntry(FPluginBuilderCommands::Get().Rocket);
		MenuBuilder.AddMenuEntry(FPluginBuilderCommands::Get().CreateSubFolder);
		MenuBuilder.AddMenuEntry(FPluginBuilderCommands::Get().StrictIncludes);
		MenuBuilder.AddMenuEntry(FPluginBuilderCommands::Get().ZipUp);
	}

	void FPluginBuilderMenuExtension::OnExtendEngineVersionsSubMenu(FMenuBuilder& MenuBuilder)
	{
		const TArray<FEngineVersions::FEngineVersion>& EngineVersions = FEngineVersions::GetEngineVersions();
		for (const auto& EngineVersion : EngineVersions)
		{
			MenuBuilder.AddMenuEntry(
            	FText::FromString(EngineVersion.VersionName),
            	FText::FromString(EngineVersion.InstalledDirectory),
            	FSlateIcon(
#if UE_5_00_OR_LATER
                	FAppStyle::GetAppStyleSetName(),
#else
                	FEditorStyle::GetStyleSetName(),
#endif
                	TEXT("MainFrame.AboutUnrealEd")
                ),
                FUIAction(
					FExecuteAction::CreateStatic(&FEngineVersions::ToggleEngineVersion, EngineVersion),
					FCanExecuteAction(),
					FIsActionChecked::CreateStatic(&FEngineVersions::GetEngineVersionState, EngineVersion)
                ),
                NAME_None,
                EUserInterfaceActionType::RadioButton
            );
		}
	}

	void FPluginBuilderMenuExtension::OnExtendTargetPlatformsSubMenu(FMenuBuilder& MenuBuilder)
	{
		const TArray<FPlatformNames::FPlatformName>& PlatformNames = FPlatformNames::GetPlatformNames();
		for (const auto& PlatformName : PlatformNames)
		{
			MenuBuilder.AddMenuEntry(
				FText::FromString(PlatformName.UBTPlatformName),
				FText::Format(
					LOCTEXT("TargetPlatformTooltipFormat", "Include {0} in the target platforms."),
					FText::FromString(PlatformName.UBTPlatformName)
				),
				FSlateIcon(
#if UE_5_00_OR_LATER
                	FAppStyle::GetAppStyleSetName(),
#else
                	FEditorStyle::GetStyleSetName(),
#endif
                	*FString::Printf(TEXT("Launcher.Platform_%s"), *PlatformName.IniPlatformName)
                ),
                FUIAction(
				   FExecuteAction::CreateStatic(&FPlatformNames::ToggleTargetPlatform, PlatformName),
				   FCanExecuteAction(),
				   FIsActionChecked::CreateStatic(&FPlatformNames::GetTargetPlatformState, PlatformName)
			   ),
			   NAME_None,
			   EUserInterfaceActionType::RadioButton
			);
		}
	}

	void FPluginBuilderMenuExtension::OnExtendBuildTargetSubMenu(FMenuBuilder& MenuBuilder)
	{
		TArray<FBuildTarget> BuildTargets = FBuildTarget::GetFilteredBuildTargets();
		for (auto& BuildTarget : BuildTargets)
		{
			MenuBuilder.AddMenuEntry(
				FText::Format(
					LOCTEXT("BuildTargetLabelFormat", "{0} ({1})"),
					FText::FromString(BuildTarget.GetPluginName()),
					FText::FromString(BuildTarget.GetPluginVersionName())
				),
				FText::FromString(BuildTarget.GetPluginDescription()),
				BuildTarget.GetPluginIcon(),
				FUIAction(
					FExecuteAction::CreateStatic(&FBuildTarget::SelectBuildTarget, BuildTarget),
					FCanExecuteAction(),
					FIsActionChecked::CreateStatic(&FBuildTarget::IsBuildTargetSelected, BuildTarget)
				),
				NAME_None,
				EUserInterfaceActionType::RadioButton
			);
		}
	}
}

#undef LOCTEXT_NAMESPACE
