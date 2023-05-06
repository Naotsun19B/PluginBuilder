// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/BuildTargets.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/App.h"

namespace PluginBuilder
{
	FBuildTargets::FBuildTarget::FBuildTarget(const TSharedRef<IPlugin>& Plugin)
	{
		const FPluginDescriptor& Descriptor = Plugin->GetDescriptor();
		PluginName = Descriptor.FriendlyName;
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
		const auto& Settings = UPluginBuilderSettings::Get();
		
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
				return (Lhs.GetPluginName() < Rhs.GetPluginName());
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

		const FBuildTarget* SavedBuildTarget = BuildTargets.FindByPredicate(
			[](const FBuildTarget& BuildTarget) -> bool
			{
				return UPluginBuilderSettings::Get().SelectedBuildTargetName.IsEqual(*BuildTarget.GetPluginName());
			}
		);
		if (SavedBuildTarget != nullptr)
		{
			return *SavedBuildTarget;
		}
		
		const FBuildTarget* DefaultBuildTarget = BuildTargets.FindByPredicate(
			[](const FBuildTarget& BuildTarget) -> bool
			{
				static const FString ProjectName = FApp::GetProjectName();
				return ProjectName.Contains(BuildTarget.GetPluginName());
			}
		);
		if (DefaultBuildTarget != nullptr)
		{
			return *DefaultBuildTarget;
		}
		
		return BuildTargets[0];
	}

	void FBuildTargets::SelectBuildTarget(const FBuildTarget BuildTarget)
	{
		auto& Settings = UPluginBuilderSettings::Get();
		Settings.SelectedBuildTargetName = *BuildTarget.GetPluginName();
		Settings.SelectedBuildTarget = BuildTarget;
		Settings.SaveConfig();
	}

	bool FBuildTargets::IsBuildTargetSelected(const FBuildTarget BuildTarget)
	{
		const TOptional<FBuildTarget>& SelectedBuildTarget = UPluginBuilderSettings::Get().SelectedBuildTarget;
		if (SelectedBuildTarget.IsSet())
		{
			return BuildTarget.GetPluginName().Equals(SelectedBuildTarget.GetValue().GetPluginName());
		}

		return false;
	}
}
