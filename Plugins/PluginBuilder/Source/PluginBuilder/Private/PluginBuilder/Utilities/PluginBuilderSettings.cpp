// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "PluginBuilderSettings"

namespace PluginBuilder
{
	namespace Settings
	{
		static const FName ContainerName    = TEXT("Editor");
		static const FName CategoryName	    = TEXT("Plugins");
		static const FName SectionName      = TEXT("PluginBuilderSettings");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		}
	}
}

UPluginBuilderSettings::UPluginBuilderSettings()
	: bSearchOnlyEnabled(true)
	, bContainsProjectPlugins(true)
	, bContainsEnginePlugins(false)
	, bSelectOutputDirectoryManually(false)
	, bStopPackagingProcessImmediately(false)
	, SelectedBuildTargetName(NAME_None)
	, bRocket(true)
	, bCreateSubFolder(false)
	, bStrictIncludes(false)
	, bZipUp(true)
{
	OutputDirectoryPath.Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Build"));
}

void UPluginBuilderSettings::Register()
{
	if (ISettingsModule* SettingsModule = PluginBuilder::Settings::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			PluginBuilder::Settings::ContainerName,
			PluginBuilder::Settings::CategoryName,
			PluginBuilder::Settings::SectionName,
			LOCTEXT("SettingName", "Plugin Builder"),
			LOCTEXT("SettingDescription", "Editor settings for Plugin Builder"),
			GetMutableDefault<UPluginBuilderSettings>()
		);
	}
}

void UPluginBuilderSettings::Unregister()
{
	if (ISettingsModule* SettingsModule = PluginBuilder::Settings::GetSettingsModule())
	{
		SettingsModule->UnregisterSettings(
			PluginBuilder::Settings::ContainerName,
			PluginBuilder::Settings::CategoryName,
			PluginBuilder::Settings::SectionName
		);
	}
}

UPluginBuilderSettings& UPluginBuilderSettings::Get()
{
	auto* Settings = GetMutableDefault<UPluginBuilderSettings>();
	check(IsValid(Settings));
	return *Settings;
}

void UPluginBuilderSettings::OpenSettings()
{
	if (ISettingsModule* SettingsModule = PluginBuilder::Settings::GetSettingsModule())
	{
		SettingsModule->ShowViewer(
			PluginBuilder::Settings::ContainerName,
			PluginBuilder::Settings::CategoryName,
			PluginBuilder::Settings::SectionName
		);
	}
}

void UPluginBuilderSettings::PostInitProperties()
{
	UObject::PostInitProperties();

	SelectedBuildTarget = PluginBuilder::FBuildTargets::GetDefaultBuildTarget();
}

bool UPluginBuilderSettings::IsReadyToStartPackagePluginTask() const
{
	return (SelectedBuildTarget.IsSet() && (EngineVersions.Num() > 0));
}

#undef LOCTEXT_NAMESPACE
