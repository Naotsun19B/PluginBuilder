// Copyright 2022-2025 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/BuildTargets.h"
#include "PluginBuilder/Utilities/PluginBuilderEditorSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderBuildConfigurationSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/App.h"

namespace PluginBuilder
{
	FBuildTargets::FBuildTarget::FBuildTarget(const TSharedRef<IPlugin>& Plugin)
	{
		const FPluginDescriptor& Descriptor = Plugin->GetDescriptor();
		PluginName = Plugin->GetName();
		PluginFriendlyName = Descriptor.FriendlyName;
		PluginDescription = Descriptor.Description;
		PluginCategory = Descriptor.Category;
		PluginVersionName = Descriptor.VersionName;
		bCanPluginContainContent = Descriptor.bCanContainContent;
		UPluginFile = FPaths::ConvertRelativePathToFull(Plugin->GetDescriptorFileName());
	}

	FString FBuildTargets::FBuildTarget::GetPluginName() const
	{
		return PluginName;
	}

	FString FBuildTargets::FBuildTarget::GetPluginFriendlyName() const
	{
		return PluginFriendlyName;
	}

	FString FBuildTargets::FBuildTarget::GetPluginNameInSpecifiedFormat() const
	{
		const auto& Settings = GetSettings<UPluginBuilderEditorSettings>();
		return (
			Settings.bUseFriendlyName ?
			GetPluginFriendlyName() :
			GetPluginName()
		);
	}

	FString FBuildTargets::FBuildTarget::GetPluginDescription() const
	{
		return PluginDescription;
	}

	FString FBuildTargets::FBuildTarget::GetPluginCategory() const
	{
		return PluginCategory;
	}

	FSlateIcon FBuildTargets::FBuildTarget::GetPluginIcon() const
	{
		return FSlateIcon(
			FPluginBuilderStyle::Get().GetStyleSetName(),
			FPluginBuilderStyle::GetPropertyName(PluginName)
		);
	}

	FString FBuildTargets::FBuildTarget::GetPluginVersionName() const
	{
		return PluginVersionName;
	}

	bool FBuildTargets::FBuildTarget::CanPluginContainContent() const
	{
		return bCanPluginContainContent;
	}

	FString FBuildTargets::FBuildTarget::GetUPluginFile() const
	{
		return UPluginFile;
	}
	
	TArray<FBuildTargets::FBuildTarget> FBuildTargets::GetFilteredBuildTargets()
	{
		const auto& Settings = GetSettings<UPluginBuilderEditorSettings>();
		
		TArray<FBuildTarget> FilteredPlugins;
	
		const TArray<TSharedRef<IPlugin>> Plugins = IPluginManager::Get().GetDiscoveredPlugins();
		for (const auto& Plugin : Plugins)
		{
			if (Settings.bSearchOnlyEnabled && !Plugin->IsEnabled())
			{
				continue;
			}
			
			const EPluginLoadedFrom LoadedFrom = Plugin->GetLoadedFrom();
			if ((!Settings.bContainsProjectPlugins && LoadedFrom == EPluginLoadedFrom::Project) ||
				(!Settings.bContainsEnginePlugins && LoadedFrom == EPluginLoadedFrom::Engine))
			{
				continue;
			}
			
			FilteredPlugins.Add(FBuildTarget(Plugin));
		}

		FilteredPlugins.Sort(
			[](const FBuildTarget& Lhs, const FBuildTarget& Rhs) -> bool
			{
				return (Lhs.GetPluginFriendlyName() < Rhs.GetPluginFriendlyName());
			}
		);

		return FilteredPlugins;
	}
	
	TOptional<FBuildTargets::FBuildTarget> FBuildTargets::GetDefaultBuildTarget()
	{
		const TArray<FBuildTarget>& BuildTargets = GetFilteredBuildTargets();
		if (BuildTargets.Num() == 0)
		{
			return {};
		}
		
		const FBuildTarget* DefaultBuildTarget = BuildTargets.FindByPredicate(
			[](const FBuildTarget& BuildTarget) -> bool
			{
				static const FString ProjectName = FApp::GetProjectName();
				return ProjectName.Contains(BuildTarget.GetPluginFriendlyName());
			}
		);
		if (DefaultBuildTarget != nullptr)
		{
			return *DefaultBuildTarget;
		}
		
		return BuildTargets[0];
	}

	TOptional<FBuildTargets::FBuildTarget> FBuildTargets::LoadBuildTarget(const FString& SelectedBuildTargetName)
	{
		const TArray<FBuildTarget>& BuildTargets = GetFilteredBuildTargets();
		if (BuildTargets.Num() == 0)
		{
			return {};
		}

		const FBuildTarget* SelectedBuildTarget = BuildTargets.FindByPredicate(
			[&](const FBuildTarget& BuildTarget) -> bool
			{
				return (BuildTarget.GetPluginFriendlyName() == SelectedBuildTargetName);
			}
		);
		if (SelectedBuildTarget == nullptr)
		{
			return {};
		}

		return *SelectedBuildTarget;
	}

	void FBuildTargets::ToggleBuildTarget(const FBuildTarget BuildTarget)
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		Settings.SelectedBuildTarget = BuildTarget;
	}

	bool FBuildTargets::GetBuildTargetState(const FBuildTarget BuildTarget)
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		const TOptional<FBuildTarget>& SelectedBuildTarget = Settings.SelectedBuildTarget;
		if (SelectedBuildTarget.IsSet())
		{
			return BuildTarget.GetPluginFriendlyName().Equals(SelectedBuildTarget.GetValue().GetPluginFriendlyName());
		}

		return false;
	}
}
