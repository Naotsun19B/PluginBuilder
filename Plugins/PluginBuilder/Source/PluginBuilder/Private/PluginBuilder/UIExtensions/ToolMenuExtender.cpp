// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/UIExtensions/ToolMenuExtender.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommands.h"
#include "PluginBuilder/Types/BuildTargets.h"
#include "PluginBuilder/Types/EngineVersions.h"
#include "PluginBuilder/Types/TargetPlatforms.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "ToolMenus.h"
#if UE_5_00_OR_LATER
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif

#define LOCTEXT_NAMESPACE "PluginBuilderMenuExtension"

namespace PluginBuilder
{
	const FName FToolMenuExtender::FilePluginSectionName			= TEXT("PluginBuilder.FilePlugin");
	const FName FToolMenuExtender::PackagePluginSubMenuName			= TEXT("PluginBuilder.PackagePlugin");
	const FName FToolMenuExtender::PackagePluginSectionName			= TEXT("PackagePlugin");
	const FName FToolMenuExtender::BuildConfigurationsSubMenuName	= TEXT("PluginBuilder.PackagePlugin.BuildConfigurations");
	const FName FToolMenuExtender::BuildTargetsSubMenuName			= TEXT("PluginBuilder.PackagePlugin.BuildTargets");
	const FName FToolMenuExtender::VersionsAndPlatformsSectionName	= TEXT("VersionsAndPlatforms");
	const FName FToolMenuExtender::EngineVersionsSubMenuName		= TEXT("PluginBuilder.PackagePlugin.BuildConfigurations.EngineVersions");
	const FName FToolMenuExtender::TargetPlatformsSubMenuName		= TEXT("PluginBuilder.PackagePlugin.BuildConfigurations.TargetPlatforms");
	const FName FToolMenuExtender::OptionsSectionName				= TEXT("Options");
	
	void FToolMenuExtender::Register()
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
			FNewToolMenuChoice(FNewToolMenuDelegate::CreateStatic(&FToolMenuExtender::OnExtendPackagePluginSubMenu)),
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

	void FToolMenuExtender::Unregister()
	{
		UToolMenu* MainFrameFileMenu = GetMenuExtensionPoint();
		if (!IsValid(MainFrameFileMenu))
		{
			return;
		}

		MainFrameFileMenu->RemoveSection(FilePluginSectionName);
	}

	UToolMenu* FToolMenuExtender::GetMenuExtensionPoint()
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

	void FToolMenuExtender::OnExtendPackagePluginSubMenu(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}

		FToolMenuSection& Section = ToolMenu->AddSection(PackagePluginSectionName);
		
		Section.AddMenuEntry(FPluginBuilderCommands::Get().BuildPlugin);
		
		Section.AddSeparator(TEXT("BeginChildSubMenus"));
		
		Section.AddSubMenu(
			BuildConfigurationsSubMenuName,
			LOCTEXT("BuildConfigurationLabel", "Build Configurations"),
			LOCTEXT("BuildConfigurationTooltip", "Select the version and platform of the engine to build, whether to zip file, etc."),
			FNewToolMenuChoice(FNewToolMenuDelegate::CreateStatic(&FToolMenuExtender::OnExtendBuildConfigurationsSubMenu)),
			false,
			{},
			false
		);

		Section.AddSubMenu(
			BuildTargetsSubMenuName,
			LOCTEXT("BuildTargetLabel", "Build Targets"),
			LOCTEXT("BuildTargetTooltip", "Select the plugin to build."),
			FNewToolMenuChoice(FNewToolMenuDelegate::CreateStatic(&FToolMenuExtender::OnExtendBuildTargetsSubMenu)),
			false,
			{},
			false
		);
		
		Section.AddSeparator(TEXT("EndChildSubMenus"));
		
		Section.AddMenuEntry(FPluginBuilderCommands::Get().OpenBuildSettings);
	}

	void FToolMenuExtender::OnExtendBuildConfigurationsSubMenu(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}

		FToolMenuSection& VersionsAndPlatformsSection = ToolMenu->AddSection(
			VersionsAndPlatformsSectionName,
			LOCTEXT("VersionsAndPlatformsLabel", "Versions And Platforms")
		);
		
		VersionsAndPlatformsSection.AddSubMenu(
			EngineVersionsSubMenuName,
			LOCTEXT("EngineVersionsLabel", "Engine Versions"),
			LOCTEXT("EngineVersionsTooltip", "Specifies the engine version to build the plugin."),
			FNewToolMenuChoice(FNewToolMenuDelegate::CreateStatic(&FToolMenuExtender::OnExtendEngineVersionsSubMenu)),
			false,
			{},
			false
		);

		VersionsAndPlatformsSection.AddSubMenu(
			TargetPlatformsSubMenuName,
			LOCTEXT("TargetPlatformsLabel", "Target Platforms"),
			LOCTEXT("TargetPlatformsTooltip", "Specifies the target platforms to build the plugin."),
			FNewToolMenuChoice(FNewToolMenuDelegate::CreateStatic(&FToolMenuExtender::OnExtendTargetPlatformsSubMenu)),
			false,
			{},
			false
		);

		FToolMenuSection& OptionsSection = ToolMenu->AddSection(
			OptionsSectionName,
			LOCTEXT("OptionsLabel", "Options")
		);
		
		OptionsSection.AddMenuEntry(FPluginBuilderCommands::Get().Rocket);
		OptionsSection.AddMenuEntry(FPluginBuilderCommands::Get().CreateSubFolder);
		OptionsSection.AddMenuEntry(FPluginBuilderCommands::Get().StrictIncludes);
		OptionsSection.AddMenuEntry(FPluginBuilderCommands::Get().ZipUp);
	}

	void FToolMenuExtender::OnExtendEngineVersionsSubMenu(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}
		
		const TArray<FEngineVersions::FEngineVersion>& EngineVersions = FEngineVersions::GetEngineVersions();
		for (const auto& EngineVersion : EngineVersions)
		{
			const FString& MajorVersionName = EngineVersion.MajorVersionName;
			FToolMenuSection& Section = ToolMenu->FindOrAddSection(*MajorVersionName);
			Section.Label = FText::FromString(MajorVersionName);
			
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
                EUserInterfaceActionType::ToggleButton
            );
		}
	}

	void FToolMenuExtender::OnExtendTargetPlatformsSubMenu(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}
		
		const TArray<FTargetPlatforms::FTargetPlatform>& PlatformNames = FTargetPlatforms::GetPlatformNames();
		for (const auto& PlatformName : PlatformNames)
		{
			const FName& PlatformGroupName = PlatformName.PlatformGroupName;
			FToolMenuSection& Section = ToolMenu->FindOrAddSection(PlatformGroupName);
			Section.Label = FText::FromName(PlatformGroupName);
			
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
				   FExecuteAction::CreateStatic(&FTargetPlatforms::ToggleTargetPlatform, PlatformName),
				   FCanExecuteAction(),
				   FIsActionChecked::CreateStatic(&FTargetPlatforms::GetTargetPlatformState, PlatformName)
			   ),
			   EUserInterfaceActionType::ToggleButton
			);
		}
	}

	void FToolMenuExtender::OnExtendBuildTargetsSubMenu(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}
		
		const TArray<FBuildTargets::FBuildTarget>& BuildTargets = FBuildTargets::GetFilteredBuildTargets();
		for (auto& BuildTarget : BuildTargets)
		{
			const FString PluginCategory = BuildTarget.GetPluginCategory();
			FToolMenuSection& Section = ToolMenu->FindOrAddSection(*PluginCategory);
			Section.Label = FText::FromString(PluginCategory);
			
			Section.AddMenuEntry(
				*BuildTarget.GetPluginFriendlyName(),
				FText::Format(
					LOCTEXT("BuildTargetLabelFormat", "{0} ({1})"),
					FText::FromString(BuildTarget.GetPluginFriendlyName()),
					FText::FromString(BuildTarget.GetPluginVersionName())
				),
				FText::FromString(BuildTarget.GetPluginDescription()),
				BuildTarget.GetPluginIcon(),
				FUIAction(
					FExecuteAction::CreateStatic(&FBuildTargets::SelectBuildTarget, BuildTarget),
					FCanExecuteAction(),
					FIsActionChecked::CreateStatic(&FBuildTargets::IsBuildTargetSelected, BuildTarget)
				),
				EUserInterfaceActionType::RadioButton
			);
		}
	}
}

#undef LOCTEXT_NAMESPACE
