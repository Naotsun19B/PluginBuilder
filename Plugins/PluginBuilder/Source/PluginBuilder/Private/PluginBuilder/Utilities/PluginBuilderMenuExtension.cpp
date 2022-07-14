// Copyright 2022 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderMenuExtension.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/Utilities/EngineVersions.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommands.h"
#include "PluginBuilder/Types/BuildTarget.h"
#include "ToolMenus.h"
#include "EditorStyle.h"
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "Interfaces/ITargetPlatform.h"

#if BEFORE_UE_4_27
#include "PlatformInfo.h"
#else
#include "Misc/DataDrivenPlatformInfoRegistry.h"
#endif

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
#if BEFORE_UE_4_27
			FToolMenuInsert(TEXT("FileProject"), EToolMenuInsertType::After)
#else
			FToolMenuInsert()
#endif
		);
		FilePluginSection.AddSubMenu(
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
#if BEFORE_UE_4_27
			TEXT("MainFrame.MainTabMenu.File");
#else
			TEXT("LevelEditor.MainMenu.Build");
#endif

		return ToolMenus->ExtendMenu(ExtensionName);
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
		TArray<FEngineVersions::FEngineVersion> InstalledEngineVersions = FEngineVersions::GetEngineVersions();
		for (auto& EngineVersion : InstalledEngineVersions)
		{
			MenuBuilder.AddMenuEntry(
				FText::FromString(EngineVersion.VersionName),
				FText::FromString(EngineVersion.InstalledDirectory),
				FSlateIcon(FEditorStyle::GetStyleSetName(), TEXT("MainFrame.AboutUnrealEd")),
				FUIAction(
					FExecuteAction::CreateLambda(
						[EngineVersion]()
						{
							auto& Settings = UPluginBuilderSettings::Get();
							
							TArray<FString>& EngineVersions = Settings.EngineVersions;
							if (EngineVersions.Contains(EngineVersion.VersionName))
							{
								EngineVersions.Remove(EngineVersion.VersionName);
							}
							else
							{
								EngineVersions.Add(EngineVersion.VersionName);
							}

							Settings.SaveConfig();
						}
					),
					FCanExecuteAction(),
					FIsActionChecked::CreateLambda(
						[EngineVersion]() -> bool
						{
							return UPluginBuilderSettings::Get().EngineVersions.Contains(EngineVersion.VersionName);
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

				FString UBTPlatformName;
				FString IniPlatformName;
				{
#if BEFORE_UE_4_27
					const PlatformInfo::FPlatformInfo& PlatformInfo = TargetPlatform->GetPlatformInfo();
					PlatformInfo.UBTTargetId.ToString(UBTPlatformName);
					IniPlatformName = PlatformInfo.IniPlatformName;
#else
					const FDataDrivenPlatformInfo& PlatformInfo = TargetPlatform->GetPlatformInfo();
					UBTPlatformName = PlatformInfo.UBTPlatformString;
					IniPlatformName = PlatformInfo.IniPlatformName.ToString();
#endif
					if (PlatformInfo.PlatformSubMenu != NAME_None)
					{
						IniPlatformName = PlatformInfo.PlatformSubMenu.ToString();
					}
				}
				
				if (IniPlatformName.IsEmpty())
				{
					continue;
				}
				
				const bool bAlreadyExists = PlatformNames.ContainsByPredicate(
					[&UBTPlatformName](const TPair<FString, FString>& PlatformName) -> bool
					{
						return PlatformName.Key.Equals(UBTPlatformName);
					}
				);
				if (!bAlreadyExists)
				{
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
