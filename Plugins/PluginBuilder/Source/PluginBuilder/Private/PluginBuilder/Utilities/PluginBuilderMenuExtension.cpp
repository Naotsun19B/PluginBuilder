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
			FNewToolMenuChoice(FNewToolMenuDelegate::CreateStatic(&FPluginBuilderMenuExtension::OnExtendPackagePluginSubMenu)),
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

	void FPluginBuilderMenuExtension::OnExtendPackagePluginSubMenu(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}

		FToolMenuSection& Section = ToolMenu->AddSection(TEXT("PackagePlugin"));
		
		Section.AddMenuEntry(FPluginBuilderCommands::Get().BuildPlugin);
		
		Section.AddSeparator(TEXT("BeginChildSubMenus"));
		
		Section.AddSubMenu(
			TEXT("BuildConfigurations"),
			LOCTEXT("BuildConfigurationLabel", "Build Configurations"),
			LOCTEXT("BuildConfigurationTooltip", "Select the version and platform of the engine to build, whether to zip file, etc."),
			FNewToolMenuChoice(FNewToolMenuDelegate::CreateStatic(&FPluginBuilderMenuExtension::OnExtendBuildConfigurationsSubMenu)),
			false,
			{},
			false
		);

		Section.AddSubMenu(
			TEXT("BuildTargets"),
			LOCTEXT("BuildTargetLabel", "Build Targets"),
			LOCTEXT("BuildTargetTooltip", "Select the plugin to build."),
			FNewToolMenuChoice(FNewToolMenuDelegate::CreateStatic(&FPluginBuilderMenuExtension::OnExtendBuildTargetsSubMenu)),
			false,
			{},
			false
		);
		
		Section.AddSeparator(TEXT("EndChildSubMenus"));
		
		Section.AddMenuEntry(FPluginBuilderCommands::Get().OpenBuildSettings);
	}

	void FPluginBuilderMenuExtension::OnExtendBuildConfigurationsSubMenu(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}

		FToolMenuSection& Section = ToolMenu->AddSection(TEXT("BuildConfigurations"));
		
		Section.AddSubMenu(
			TEXT("EngineVersions"),
			LOCTEXT("EngineVersionsLabel", "Engine Versions"),
			LOCTEXT("EngineVersionsTooltip", "Specifies the engine version to build the plugin."),
			FNewToolMenuChoice(FNewToolMenuDelegate::CreateStatic(&FPluginBuilderMenuExtension::OnExtendEngineVersionsSubMenu)),
			false,
			{},
			false
		);

		Section.AddSubMenu(
			TEXT("TargetPlatforms"),
			LOCTEXT("TargetPlatformsLabel", "Target Platforms"),
			LOCTEXT("TargetPlatformsTooltip", "Specifies the target platforms to build the plugin."),
			FNewToolMenuChoice(FNewToolMenuDelegate::CreateStatic(&FPluginBuilderMenuExtension::OnExtendTargetPlatformsSubMenu)),
			false,
			{},
			false
		);

		Section.AddMenuEntry(FPluginBuilderCommands::Get().Rocket);
		Section.AddMenuEntry(FPluginBuilderCommands::Get().CreateSubFolder);
		Section.AddMenuEntry(FPluginBuilderCommands::Get().StrictIncludes);
		Section.AddMenuEntry(FPluginBuilderCommands::Get().ZipUp);
	}

	void FPluginBuilderMenuExtension::OnExtendEngineVersionsSubMenu(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}

		FToolMenuSection& Section = ToolMenu->AddSection(TEXT("EngineVersions"));
		
		const TArray<FEngineVersions::FEngineVersion>& EngineVersions = FEngineVersions::GetEngineVersions();
		for (const auto& EngineVersion : EngineVersions)
		{
			Section.AddMenuEntry(
				*EngineVersion.VersionName,
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
                EUserInterfaceActionType::RadioButton
            );
		}
	}

	void FPluginBuilderMenuExtension::OnExtendTargetPlatformsSubMenu(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}

		FToolMenuSection& Section = ToolMenu->AddSection(TEXT("TargetPlatforms"));
		
		const TArray<FPlatformNames::FPlatformName>& PlatformNames = FPlatformNames::GetPlatformNames();
		for (const auto& PlatformName : PlatformNames)
		{
			Section.AddMenuEntry(
				*PlatformName.UBTPlatformName,
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
			   EUserInterfaceActionType::RadioButton
			);
		}
	}

	void FPluginBuilderMenuExtension::OnExtendBuildTargetsSubMenu(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}

		FToolMenuSection& Section = ToolMenu->AddSection(TEXT("BuildTarget"));
		
		TArray<FBuildTarget> BuildTargets = FBuildTarget::GetFilteredBuildTargets();
		for (auto& BuildTarget : BuildTargets)
		{
			Section.AddMenuEntry(
				*BuildTarget.GetPluginName(),
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
				EUserInterfaceActionType::RadioButton
			);
		}
	}
}

#undef LOCTEXT_NAMESPACE
