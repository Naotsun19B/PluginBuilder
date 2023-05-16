// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/IPluginBuilder.h"
#include "PluginBuilder/PluginBuilderGlobals.h"

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
				if (!PluginBuilder::FPackagePluginParams::MakeFromPluginFriendlyName(PluginBuilder::Global::PluginName, Params))
				{
					return;
				}
				
				Params.EngineVersions.Add(TEXT("5.1"));
				Params.BuildPluginParams.bRocket = true;
				Params.BuildPluginParams.bCreateSubFolder = false;
				Params.BuildPluginParams.bStrictIncludes = true;
				Params.BuildPluginParams.bZipUp = true;
				
				PluginBuilder::IPluginBuilder::Get().StartPackagePluginTask(Params);
			}
		)
	);
}
