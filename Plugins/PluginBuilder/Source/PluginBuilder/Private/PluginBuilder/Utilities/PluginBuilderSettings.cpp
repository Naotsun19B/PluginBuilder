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
		static const FName ContainerName	= TEXT("Editor");
		static const FName CategoryName		= TEXT("Plugins");
		static const FName SectionName		= TEXT("PluginBuilderSettings");

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
	, bOutputToBuildDirectoryOfEachProject(false)
	, bUseFriendlyName(true)
	, bShowOnlyLogsFromThisPluginWhenPackageProcessStarts(false)
	, bStopPackagingProcessImmediately(false)
	, SelectedBuildTargetName(NAME_None)
	, bRocket(true)
	, bCreateSubFolder(false)
	, bStrictIncludes(false)
	, bZipUp(true)
	, bOutputAllZipFilesToSingleFolder(false)
	, bKeepBinariesFolder(false)
	, bKeepUPluginProperties(false)
{
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

const UPluginBuilderSettings& UPluginBuilderSettings::Get()
{
	const auto* Settings = GetDefault<UPluginBuilderSettings>();
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

void UPluginBuilderSettings::ModifyProperties(
	const TFunction<void(UPluginBuilderSettings& Settings)>& Predicate,
	const EPostModifiedProcessing PostModifiedProcessing /* = EPostModifiedProcessing::None */
)
{
	auto* Settings = GetMutableDefault<UPluginBuilderSettings>();
	check(IsValid(Settings));
	
	Predicate(*Settings);

	if (PostModifiedProcessing == EPostModifiedProcessing::SortEngineVersion)
	{
		Settings->EngineVersions.Sort(
			[](const FString& Lhs, const FString& Rhs) -> bool
			{
				const float LhsValue = FCString::Atof(*Lhs);
				const float RhsValue = FCString::Atof(*Rhs);
				return (LhsValue < RhsValue);
			}
		);
	}

	Settings->SaveConfig();
}

void UPluginBuilderSettings::PostInitProperties()
{
	UObject::PostInitProperties();

	ResetOutputDirectoryPath();
	
	SelectedBuildTarget = PluginBuilder::FBuildTargets::GetDefaultBuildTarget();
}

void UPluginBuilderSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPluginBuilderSettings, OutputDirectoryPath) ||
		PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPluginBuilderSettings, bOutputToBuildDirectoryOfEachProject))
	{
		ResetOutputDirectoryPath();
	}
}

bool UPluginBuilderSettings::CanEditChange(const FProperty* InProperty) const
{
	if (!UObject::CanEditChange(InProperty) || (InProperty == nullptr))
	{
		return false;
	}

#if !UE_5_01_OR_LATER
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPluginBuilderSettings, bShowOnlyLogsFromThisPluginWhenPackageProcessStarts))
	{
		return false;
	}
#endif

	return true;
}

void UPluginBuilderSettings::ResetOutputDirectoryPath()
{
	if (OutputDirectoryPath.Path.IsEmpty() || bOutputToBuildDirectoryOfEachProject)
	{
		OutputDirectoryPath.Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Build"));
	}
}

bool UPluginBuilderSettings::IsReadyToStartPackagePluginTask() const
{
	return (SelectedBuildTarget.IsSet() && (EngineVersions.Num() > 0));
}

#undef LOCTEXT_NAMESPACE
