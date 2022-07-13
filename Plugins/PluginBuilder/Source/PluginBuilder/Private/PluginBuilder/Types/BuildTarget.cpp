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

			const FPluginDescriptor& Descriptor = Plugin->GetDescriptor();
			FilteredPlugins.Add(
				FBuildTarget(
					Descriptor.FriendlyName,
					Descriptor.Description
				)
			);
		}

		return FilteredPlugins;
	}
	
	TOptional<FBuildTarget> FBuildTarget::GetDefaultBuildTarget()
	{
		const TArray<FBuildTarget>& BuildTargets = GetFilteredBuildTargets();
		if (BuildTargets.Num() == 0)
		{
			return {};
		}
		
		const FString ProjectName = FApp::GetProjectName();
		for (const auto& BuildTarget : BuildTargets)
		{
			if (ProjectName.Contains(BuildTarget.PluginName))
			{
				return BuildTarget;
			}
		}

		return BuildTargets[0];
	}

	void FBuildTarget::SelectBuildTarget(const FBuildTarget BuildTarget)
	{
		UPluginBuilderSettings::Get().SelectedBuildTarget = BuildTarget;
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

	FBuildTarget::FBuildTarget(const FString& InPluginName, const FString InPluginDescription)
		: PluginName(InPluginName)
		, PluginDescription(InPluginDescription)
	{
	}

	FText FBuildTarget::GetPluginName() const
	{
		return FText::FromString(PluginName);
	}

	FText FBuildTarget::GetPluginDescription() const
	{
		return FText::FromString(PluginDescription);
	}

	FSlateIcon FBuildTarget::GetPluginIcon() const
	{
		return FSlateIcon(
			FPluginBuilderStyle::Get().GetStyleSetName(),
			FPluginBuilderStyle::GetPropertyName(PluginName)
		);
	}
}
