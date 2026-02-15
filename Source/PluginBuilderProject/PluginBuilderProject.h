// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/IPluginBuilder.h"

namespace PluginBuilderTest
{
	static FAutoConsoleCommand StartPackagePluginTaskWithEditorPreferenceParams(
		TEXT("PluginBuilder.StartPackagePluginTask.EditorPreference"),
		TEXT("Start a package plugin task with parameters set in the editor preferences."),
		FConsoleCommandDelegate::CreateLambda(
			[]()
			{
				PluginBuilder::IPluginBuilder::Get().StartPackagePluginTask();
			}
		)
	);
	
	static FAutoConsoleCommand StartPackagePluginTaskWithCustomParams(
		TEXT("PluginBuilder.StartPackagePluginTask.CustomParams"),
		TEXT("Start a package plugin task with custom parameters set in the code."),
		FConsoleCommandDelegate::CreateLambda(
			[]()
			{
				PluginBuilder::FPackagePluginParams Params;
				if (!PluginBuilder::FPackagePluginParams::MakeFromPluginFriendlyName(PluginBuilder::IPluginBuilder::ModuleName, Params))
				{
					return;
				}

				PluginBuilder::FBuildPluginParams BuildPluginParams;
				BuildPluginParams.bRocket = true;
				BuildPluginParams.bCreateSubFolder = false;
				BuildPluginParams.bStrictIncludes = true;
				BuildPluginParams.bNoHostPlatform = false;

				PluginBuilder::FZipUpPluginParams ZipUpPluginParams;
				ZipUpPluginParams.bKeepBinariesFolder = true;
				ZipUpPluginParams.CompressionLevel = 8;
				
				Params.EngineVersions.Add(TEXT("5.5"));
				Params.EngineVersions.Add(TEXT("5.7"));
				Params.BuildPluginParams = BuildPluginParams;
				Params.ZipUpPluginParams = ZipUpPluginParams;
				
				PluginBuilder::IPluginBuilder::Get().StartPackagePluginTask(Params);
			}
		)
	);
}
