// Copyright 2022 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderMenuExtension.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommands.h"
#include "PluginBuilder/Types/BuildTarget.h"
#include "ToolMenus.h"
#include "EditorStyle.h"
#include "Misc/DataDrivenPlatformInfoRegistry.h"

#define LOCTEXT_NAMESPACE "PluginBuilderMenuExtension"

namespace PluginBuilder
{
	const FName FPluginBuilderMenuExtension::FilePluginSectionName		= TEXT("PluginBuilder.FilePlugin");
	const FName FPluginBuilderMenuExtension::PackagePluginSubMenuName	= TEXT("PluginBuilder.PackagePlugin");
	
	void FPluginBuilderMenuExtension::Register()
	{
		UToolMenu* MainFrameFileMenu = GetMainFrameFileMenu();
		if (!IsValid(MainFrameFileMenu))
		{
			return;
		}
		
		FToolMenuSection& FileProjectSection = MainFrameFileMenu->AddSection(
			FilePluginSectionName,
			LOCTEXT("SectionLabel", "Plugin"),
			FToolMenuInsert(TEXT("FileProject"), EToolMenuInsertType::After)
		);
		FileProjectSection.AddSubMenu(
			PackagePluginSubMenuName,
			LOCTEXT("PackagePluginLabel", "Package Plugin"),
			LOCTEXT("PackagePluginTooltip", "Build the plugin with multiple engine versions and zip the required files for distribution."),
			FNewToolMenuChoice(FNewMenuDelegate::CreateStatic(&OnExtendPackagePluginSubMenu)),
			false,
			FSlateIcon(FEditorStyle::GetStyleSetName(), TEXT("MainFrame.PackageProject"))
		);
	}

	void FPluginBuilderMenuExtension::Unregister()
	{
		UToolMenu* MainFrameFileMenu = GetMainFrameFileMenu();
		if (!IsValid(MainFrameFileMenu))
		{
			return;
		}

		MainFrameFileMenu->RemoveSection(FilePluginSectionName);
	}

	UToolMenu* FPluginBuilderMenuExtension::GetMainFrameFileMenu()
	{
		UToolMenus* ToolMenus = UToolMenus::Get();
		if (!IsValid(ToolMenus))
		{
			return nullptr;
		}
		
		return ToolMenus->ExtendMenu(TEXT("MainFrame.MainTabMenu.File"));
	}

	void FPluginBuilderMenuExtension::OnExtendPackagePluginSubMenu(FMenuBuilder& MenuBuilder)
	{
		MenuBuilder.AddMenuEntry(FPluginBuilderCommands::Get().BuildPlugin);

		MenuBuilder.AddSeparator();
		
		MenuBuilder.AddSubMenu(
			LOCTEXT("BuildConfigurationLabel", "Build Configuration"),
			LOCTEXT("BuildConfigurationTooltip", "Select the version and platform of the engine to build, whether to zip file, etc."),
			FNewMenuDelegate::CreateStatic(&OnExtendBuildConfigurationSubMenu)
		);

		MenuBuilder.AddSubMenu(
			LOCTEXT("BuildTargetLabel", "Build Target"),
			LOCTEXT("BuildTargetTooltip", "Select the plugin to build."),
			FNewMenuDelegate::CreateStatic(&OnExtendBuildTargetSubMenu)
		);
		
		MenuBuilder.AddSeparator();

		MenuBuilder.AddMenuEntry(FPluginBuilderCommands::Get().OpenBuildSettings);
	}

	void FPluginBuilderMenuExtension::OnExtendBuildConfigurationSubMenu(FMenuBuilder& MenuBuilder)
	{
		MenuBuilder.AddSubMenu(
			LOCTEXT("EngineVersionsLabel", "Engine Versions"),
			LOCTEXT("EngineVersionsTooltip", "Specifies the engine version to build the plugin."),
			FNewMenuDelegate::CreateStatic(&OnExtendEngineVersionsSubMenu)
		);

		MenuBuilder.AddSubMenu(
			LOCTEXT("TargetPlatformsLabel", "Target Platforms"),
			LOCTEXT("TargetPlatformsTooltip", "Specifies the target platforms to build the plugin."),
			FNewMenuDelegate::CreateStatic(&OnExtendTargetPlatformsSubMenu)
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("RocketLabel", "Rocket"),
			LOCTEXT("RocketTooltip", "Whether to handle older versions that do not use the Rocket.txt file."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda(
					[]()
					{
						auto& Settings = UPluginBuilderSettings::Get();
						Settings.bRocket = !Settings.bRocket;
					}
				),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda(
					[]()
					{
						return UPluginBuilderSettings::Get().bRocket;
					}
				)
			),
			NAME_None,
			EUserInterfaceActionType::RadioButton
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("CreateSubFolderLabel", "Create Sub Folder"),
			LOCTEXT("CreateSubFolderTooltip", "Whether to create a subfolder in the output built plugins folder."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda(
					[]()
					{
						auto& Settings = UPluginBuilderSettings::Get();
						Settings.bCreateSubFolder = !Settings.bCreateSubFolder;
					}
				),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda(
					[]()
					{
						return UPluginBuilderSettings::Get().bCreateSubFolder;
					}
				)
			),
			NAME_None,
			EUserInterfaceActionType::RadioButton
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("StrictIncludesLabel", "Strict Includes"),
			LOCTEXT("StrictIncludesTooltip", "Whether to judge the header inclusion of the plug-in code strictly."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda(
					[]()
					{
						auto& Settings = UPluginBuilderSettings::Get();
						Settings.bStrictIncludes = !Settings.bStrictIncludes;
					}
				),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda(
					[]()
					{
						return UPluginBuilderSettings::Get().bStrictIncludes;
					}
				)
			),
			NAME_None,
			EUserInterfaceActionType::RadioButton
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("ZipUpLabel", "Zip Up"),
			LOCTEXT("ZipUpTooltip", "Whether to create a zip file that contains only the files we need after the build."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda(
					[]()
					{
						auto& Settings = UPluginBuilderSettings::Get();
						Settings.bZipUp = !Settings.bZipUp;
					}
				),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda(
					[]()
					{
						return UPluginBuilderSettings::Get().bZipUp;
					}
				)
			),
			NAME_None,
			EUserInterfaceActionType::RadioButton
		);
	}

	void FPluginBuilderMenuExtension::OnExtendEngineVersionsSubMenu(FMenuBuilder& MenuBuilder)
	{
		// #TODO: Get the installed engine version from the registry.
		static const TArray<FString> InstalledEngineVersions = {
			TEXT("4.26"), TEXT("4.27"), TEXT("5.0")
		};

		for (auto& EngineVersion : InstalledEngineVersions)
		{
			MenuBuilder.AddMenuEntry(
				FText::FromString(EngineVersion),
				FText::Format(LOCTEXT("EngineVersionTooltipFormat", "Include {0} in the build version."), FText::FromString(EngineVersion)),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateLambda(
						[EngineVersion]()
						{
							auto& Settings = UPluginBuilderSettings::Get();
							
							TArray<FString>& EngineVersions = Settings.EngineVersions;
							if (EngineVersions.Contains(EngineVersion))
							{
								EngineVersions.Remove(EngineVersion);
							}
							else
							{
								EngineVersions.Add(EngineVersion);
							}

							Settings.SaveConfig();
						}
					),
					FCanExecuteAction(),
					FIsActionChecked::CreateLambda(
						[EngineVersion]() -> bool
						{
							return UPluginBuilderSettings::Get().EngineVersions.Contains(EngineVersion);
						}
					)
				),
				NAME_None,
				EUserInterfaceActionType::RadioButton
			);
		}
	}

	void FPluginBuilderMenuExtension::OnExtendTargetPlatformsSubMenu(FMenuBuilder& MenuBuilder)
	{
		TArray<FString> PlatformNames;
		for (const auto& Pair : FDataDrivenPlatformInfoRegistry::GetAllPlatformInfos())
		{
			PlatformNames.Append(Pair.Value.AllUBTPlatformNames);
		}
		
		for (const auto& PlatformName : PlatformNames)
		{
			MenuBuilder.AddMenuEntry(
				FText::FromString(PlatformName),
				FText::Format(LOCTEXT("TargetPlatformTooltipFormat", "Include {0} in the target platforms."), FText::FromString(PlatformName)),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateLambda(
						[PlatformName]()
						{
							auto& Settings = UPluginBuilderSettings::Get();
							
							TArray<FString>& TargetPlatforms = Settings.TargetPlatforms;
							if (TargetPlatforms.Contains(PlatformName))
							{
								TargetPlatforms.Remove(PlatformName);
							}
							else
							{
								TargetPlatforms.Add(PlatformName);
							}

							Settings.SaveConfig();
						}
					),
					FCanExecuteAction(),
					FIsActionChecked::CreateLambda(
						[PlatformName]() -> bool
						{
							return UPluginBuilderSettings::Get().TargetPlatforms.Contains(PlatformName);
						}
					)
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
				BuildTarget.GetPluginName(),
				BuildTarget.GetPluginDescription(),
				FSlateIcon(), // #TODO: I want to set the icon of the plug-in.
				FUIAction(
					FExecuteAction::CreateStatic(&FBuildTarget::SelectBuildTarget, BuildTarget),
					FCanExecuteAction(),
					FIsActionChecked::CreateStatic(&FBuildTarget::IsSelectedBuildTarget, BuildTarget)
				),
				NAME_None,
				EUserInterfaceActionType::RadioButton
			);
		}
	}
}

#undef LOCTEXT_NAMESPACE
