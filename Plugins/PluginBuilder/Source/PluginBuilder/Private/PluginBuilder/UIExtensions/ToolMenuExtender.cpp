// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/UIExtensions/ToolMenuExtender.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommands.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/Types/BuildTargets.h"
#include "PluginBuilder/Types/EngineVersions.h"
#include "PluginBuilder/Types/TargetPlatforms.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "ToolMenus.h"
#include "Widgets/Input/SNumericEntryBox.h"
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
	const FName FToolMenuExtender::BuildOptionsSectionName			= TEXT("BuildOptions");
	const FName FToolMenuExtender::ZipUpOptionsSectionName			= TEXT("ZipUpOptions");
	const FName FToolMenuExtender::EngineVersionPresetSectionName	= TEXT("EngineVersionPreset");
	
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

		FToolMenuSection& BuildOptionsSection = ToolMenu->AddSection(
			BuildOptionsSectionName,
			LOCTEXT("BuildOptionsLabel", "Build Options")
		);
		
		BuildOptionsSection.AddMenuEntry(FPluginBuilderCommands::Get().Rocket);
		BuildOptionsSection.AddMenuEntry(FPluginBuilderCommands::Get().CreateSubFolder);
		BuildOptionsSection.AddMenuEntry(FPluginBuilderCommands::Get().StrictIncludes);

		FToolMenuSection& ZipUpOptionsSection = ToolMenu->AddSection(
			ZipUpOptionsSectionName,
			LOCTEXT("ZipUpOptionsLabel", "Zip Up Options")
		);
		
		ZipUpOptionsSection.AddMenuEntry(FPluginBuilderCommands::Get().ZipUp);
		ZipUpOptionsSection.AddMenuEntry(FPluginBuilderCommands::Get().OutputAllZipFilesToSingleFolder);
		ZipUpOptionsSection.AddMenuEntry(FPluginBuilderCommands::Get().KeepBinariesFolder);
		ZipUpOptionsSection.AddMenuEntry(FPluginBuilderCommands::Get().KeepUPluginProperties);
		{
			const TSharedRef<SNumericEntryBox<uint8>> CompressionLevelWidget = SNew(SNumericEntryBox<uint8>)
				.AllowSpin(true)
				.MinValue(0)
				.MaxValue(9)
				.MinSliderValue(0)
				.MaxSliderValue(9)
				.IsEnabled_Lambda(
					[]() -> bool
					{
						return UPluginBuilderSettings::Get().bZipUp;
					}
				)
				.Value_Lambda(
					[]() -> uint8
					{
						return UPluginBuilderSettings::Get().CompressionLevel;
					}
				)
				.OnValueChanged_Lambda(
					[](const uint8 Value)
					{
						UPluginBuilderSettings::ModifyProperties(
							[&](UPluginBuilderSettings& Settings)
							{
								Settings.CompressionLevel = Value;
							}
						);
					}
				);
			
			const FName VariableName = GET_MEMBER_NAME_CHECKED(UPluginBuilderSettings, CompressionLevel);
#if UE_5_02_OR_LATER
			FText VariableTooltip;
			for (const auto* ByteProperty : TFieldRange<FByteProperty>(UPluginBuilderSettings::StaticClass()))
			{
				if (ByteProperty == nullptr)
				{
					continue;
				}

				if (ByteProperty->GetFName() != VariableName)
				{
					continue;
				}

				VariableTooltip = ByteProperty->GetToolTipText();
				break;
			}
#endif
			
			ZipUpOptionsSection.AddEntry(
				FToolMenuEntry::InitWidget(
					VariableName,
					CompressionLevelWidget,
					FText::FromName(VariableName)
#if UE_5_02_OR_LATER
					,
					false,
					true,
					false,
					VariableTooltip
#endif
				)
			);
		}
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

		FToolMenuSection& EngineVersionPresetSection = ToolMenu->AddSection(
			EngineVersionPresetSectionName,
			LOCTEXT("EngineVersionPresetSectionLabel", "Preset")
		);

		EngineVersionPresetSection.AddMenuEntry(
			GET_FUNCTION_NAME_CHECKED(FEngineVersions, EnableAllEngineVersions),
			LOCTEXT("EnableAllEngineVersionsPresetLabel", "All Engine Versions"),
			LOCTEXT("EnableAllEngineVersionsPresetTooltip", "Enables all installed engine versions."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateStatic(&FEngineVersions::EnableAllEngineVersions)
			),
			EUserInterfaceActionType::Button
		);

		const TArray<FString>& MajorVersionNames = FEngineVersions::GetMajorVersionNames();
		for (const auto& MajorVersionName : MajorVersionNames)
		{
			EngineVersionPresetSection.AddMenuEntry(
				*MajorVersionName,
				FText::FromString(MajorVersionName),
				FText::Format(
					LOCTEXT("MajorVersionPresetTooltipFormat", "Enable only all engine versions belonging to {0} category."),
					FText::FromString(MajorVersionName)
				),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateStatic(&FEngineVersions::EnableByMajorVersion, MajorVersionName)
				),
				EUserInterfaceActionType::Button
			);
		}

		EngineVersionPresetSection.AddMenuEntry(
			GET_FUNCTION_NAME_CHECKED(FEngineVersions, EnableLatest3EngineVersions),
			LOCTEXT("Latest3EngineVersionsPresetLabel", "Latest 3 Engine Versions"),
			LOCTEXT("Latest3EngineVersionsPresetTooltip", "Enables the latest 3 engine versions required when submitting to the marketplace."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateStatic(&FEngineVersions::EnableLatest3EngineVersions)
			),
			EUserInterfaceActionType::Button
		);
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
				*BuildTarget.GetPluginName(),
				FText::Format(
					LOCTEXT("BuildTargetLabelFormat", "{0} ({1})"),
					FText::FromString(BuildTarget.GetPluginNameInSpecifiedFormat()),
					FText::FromString(BuildTarget.GetPluginVersionName())
				),
				FText::FromString(BuildTarget.GetPluginDescription()),
				BuildTarget.GetPluginIcon(),
				FUIAction(
					FExecuteAction::CreateStatic(&FBuildTargets::ToggleBuildTarget, BuildTarget),
					FCanExecuteAction(),
					FIsActionChecked::CreateStatic(&FBuildTargets::GetBuildTargetState, BuildTarget)
				),
				EUserInterfaceActionType::ToggleButton
			);
		}
	}
}

#undef LOCTEXT_NAMESPACE
