// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Tasks/BuildPluginTask.h"
#include "PluginBuilder/PluginBuilderGlobals.h"

namespace PluginBuilder
{
	FBuildPluginTask::FBuildPluginTask(
		const FString& InEngineVersion,
		const FUATBatchFileParams& InUATBatchFileParams,
		const FBuildPluginParams& InBuildPluginParams
	)
		: IUATBatchFileTask(InEngineVersion, InUATBatchFileParams)
		, BuildPluginParams(InBuildPluginParams)
	{
	}
	
	void FBuildPluginTask::Initialize()
	{
		UE_LOG(LogPluginBuilder, Log, TEXT("===================================================================================================="));
		UE_LOG(LogPluginBuilder, Log, TEXT("[Plugin Name] %s / [Plugin Version] %s / [Engine Version] %s"), *UATBatchFileParams.GetPluginNameInSpecifiedFormat(), *UATBatchFileParams.PluginVersionName, *EngineVersion);
		UE_LOG(LogPluginBuilder, Log, TEXT("----------------------------------------------------------------------------------------------------"));
		
		IUATBatchFileTask::Initialize();
	}

	TArray<FString> FBuildPluginTask::GetUATArguments() const
	{
		TArray<FString> Arguments = {
			TEXT("BuildPlugin"),
			FString::Printf(TEXT("-Plugin=\"%s\""), *UATBatchFileParams.UPluginFile),
			FString::Printf(TEXT("-Package=\"%s\""), *GetBuiltPluginDestinationPath())
		};
		if (BuildPluginParams.TargetPlatforms.Num() > 0)
		{
			Arguments.Add(
				FString::Printf(
						TEXT("-TargetPlatform=\"%s\""),
						*FString::Join(BuildPluginParams.TargetPlatforms, TEXT("+")
					)
				)
			);
		}
		if (BuildPluginParams.bRocket)
		{
			Arguments.Add(TEXT("-Rocket"));
		}
		if (BuildPluginParams.bCreateSubFolder)
		{
			Arguments.Add(TEXT("-CreateSubFolder"));
		}
		if (BuildPluginParams.bStrictIncludes)
		{
			Arguments.Add(TEXT("-StrictIncludes"));
		}

		return Arguments;
	}

	FString FBuildPluginTask::GetDestinationDirectoryPath() const
	{
		return GetBuiltPluginDestinationPath();
	}
}
