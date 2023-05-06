// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PackagePluginParams.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"

namespace PluginBuilder
{
	bool FPackagePluginParams::MakeDefault(FPackagePluginParams& Default)
	{
		const auto& Settings = UPluginBuilderSettings::Get();
		if (!Settings.SelectedBuildTarget.IsSet())
		{
			return false;
		}

		Default.EngineVersions = Settings.EngineVersions;

		FBuildPluginParams BuildPluginParams;
		{
			const FBuildTargets::FBuildTarget& BuildTarget = Settings.SelectedBuildTarget.GetValue();
			BuildPluginParams.PluginName = BuildTarget.GetPluginName();
			BuildPluginParams.PluginVersionName = BuildTarget.GetPluginVersionName();
			BuildPluginParams.bCanPluginContainContent = BuildTarget.CanPluginContainContent();
			BuildPluginParams.UPluginFile = BuildTarget.GetUPluginFile();
			BuildPluginParams.TargetPlatforms = Settings.TargetPlatforms;
			BuildPluginParams.bRocket = Settings.bRocket;
			BuildPluginParams.bCreateSubFolder = Settings.bCreateSubFolder;
			BuildPluginParams.bStrictIncludes = Settings.bStrictIncludes;
			BuildPluginParams.bZipUp = Settings.bZipUp;
			BuildPluginParams.bStopPackagingProcessImmediately = Settings.bStopPackagingProcessImmediately;
			if (!Settings.bSelectOutputDirectoryManually)
			{
				BuildPluginParams.OutputDirectoryPath = Settings.OutputDirectoryPath.Path;
			}
		}
		
		Default.BuildPluginParams = BuildPluginParams;

		return true;
	}
}
