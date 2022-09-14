// Copyright 2022 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/BuildTarget.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/App.h"

namespace PluginBuilder
{
	TArray<FBuildTarget> FBuildTarget::GetFilteredBuildTargets()
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
				return (Lhs.PluginName < Rhs.PluginName);
			}
		);

		return FilteredPlugins;
	}
	
	TOptional<FBuildTarget> FBuildTarget::GetDefaultBuildTarget()
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
				return ProjectName.Contains(BuildTarget.PluginName);
			}
		);
		if (DefaultBuildTarget != nullptr)
		{
			return *DefaultBuildTarget;
		}
		
		return {};
	}

	void FBuildTarget::SelectBuildTarget(const FBuildTarget BuildTarget)
	{
		auto& Settings = UPluginBuilderSettings::Get();
		Settings.SelectedBuildTargetName = *BuildTarget.GetPluginName();
		Settings.SelectedBuildTarget = BuildTarget;
		Settings.SaveConfig();
	}

	bool FBuildTarget::IsSelectedBuildTarget(const FBuildTarget BuildTarget)
	{
		const TOptional<FBuildTarget>& SelectedBuildTarget = UPluginBuilderSettings::Get().SelectedBuildTarget;
		if (SelectedBuildTarget.IsSet())
		{
			return BuildTarget.PluginName.Equals(SelectedBuildTarget.GetValue().PluginName);
		}

		return false;
	}

	FBuildTarget::FBuildTarget(const TSharedRef<IPlugin>& Plugin)
	{
		const FPluginDescriptor& Descriptor = Plugin->GetDescriptor();
		PluginName = Descriptor.FriendlyName;
		PluginDescription = Descriptor.Description;
		PluginVersionName = Descriptor.VersionName;
		bCanContainContent = Descriptor.bCanContainContent;
		UPluginFile = FPaths::ConvertRelativePathToFull(Plugin->GetDescriptorFileName());
	}

	FString FBuildTarget::GetPluginName() const
	{
		return PluginName;
	}

	FString FBuildTarget::GetPluginDescription() const
	{
		return PluginDescription;
	}

	FSlateIcon FBuildTarget::GetPluginIcon() const
	{
		return FSlateIcon(
			FPluginBuilderStyle::Get().GetStyleSetName(),
			FPluginBuilderStyle::GetPropertyName(PluginName)
		);
	}

	FString FBuildTarget::GetPluginVersionName() const
	{
		return PluginVersionName;
	}

	bool FBuildTarget::CanContainContent() const
	{
		return bCanContainContent;
	}

	FString FBuildTarget::GetUPluginFile() const
	{
		return UPluginFile;
	}
}
