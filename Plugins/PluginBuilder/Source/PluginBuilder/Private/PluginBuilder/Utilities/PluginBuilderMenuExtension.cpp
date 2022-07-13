// Copyright 2022 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderMenuExtension.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommands.h"
#include "PluginBuilder/Types/BuildTarget.h"
#include "ToolMenus.h"
#include "EditorStyle.h"
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "Interfaces/ITargetPlatform.h"
#include "PlatformInfo.h"

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
				FSlateIcon(FEditorStyle::GetStyleSetName(), TEXT("MainFrame.AboutUnrealEd")),
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
		TArray<TPair<FString, FString>> PlatformNames;
		{
			const TArray<ITargetPlatform*>& TargetPlatforms = GetTargetPlatformManager()->GetTargetPlatforms();
			for (const ITargetPlatform* TargetPlatform : TargetPlatforms)
			{
				if (TargetPlatform == nullptr)
				{
					continue;
				}

				const PlatformInfo::FPlatformInfo& PlatformInfo = TargetPlatform->GetPlatformInfo();
				if (PlatformInfo.IniPlatformName.IsEmpty())
				{
					continue;
				}

				FString UBTPlatformName;
				PlatformInfo.UBTTargetId.ToString(UBTPlatformName);
				
				const bool bAlreadyExists = PlatformNames.ContainsByPredicate(
					[&UBTPlatformName](const TPair<FString, FString>& PlatformName) -> bool
					{
						return PlatformName.Key.Equals(UBTPlatformName);
					}
				);

				if (!bAlreadyExists)
				{
					FString IniPlatformName = PlatformInfo.IniPlatformName;
					if (PlatformInfo.PlatformSubMenu != NAME_None)
	                {
	                    IniPlatformName = PlatformInfo.PlatformSubMenu.ToString();
	                }
					
					PlatformNames.Add(
						TPair<FString, FString>(
							UBTPlatformName, IniPlatformName
						)
					);
				}
			}
		}
		
		for (const auto& PlatformName : PlatformNames)
		{
			const FString UBTPlatformName = PlatformName.Key;
			const FString IniPlatformName = PlatformName.Value;
			
			MenuBuilder.AddMenuEntry(
				FText::FromString(UBTPlatformName),
				FText::Format(
					LOCTEXT("TargetPlatformTooltipFormat", "Include {0} in the target platforms."),
					FText::FromString(UBTPlatformName)
				),
				FSlateIcon(
					FEditorStyle::GetStyleSetName(),
					*FString::Printf(TEXT("Launcher.Platform_%s"), *IniPlatformName)
				),
				FUIAction(
					FExecuteAction::CreateLambda(
						[UBTPlatformName]()
						{
							auto& Settings = UPluginBuilderSettings::Get();
						
							TArray<FString>& TargetPlatforms = Settings.TargetPlatforms;
							if (TargetPlatforms.Contains(UBTPlatformName))
							{
								TargetPlatforms.Remove(UBTPlatformName);
							}
							else
							{
								TargetPlatforms.Add(UBTPlatformName);
							}

							Settings.SaveConfig();
						}
					),
					FCanExecuteAction(),
					FIsActionChecked::CreateLambda(
						[UBTPlatformName]() -> bool
						{
							return UPluginBuilderSettings::Get().TargetPlatforms.Contains(UBTPlatformName);
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
				BuildTarget.GetPluginIcon(),
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
