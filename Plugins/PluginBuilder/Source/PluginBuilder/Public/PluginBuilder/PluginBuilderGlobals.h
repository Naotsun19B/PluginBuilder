// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"

namespace PluginBuilder
{
	namespace Global
	{
		// The name of this plugin.
		static const FName PluginName = TEXT("PluginBuilder");
	}
}

/**
 * Macros to support each engine version.
 */
#ifndef UE_5_02_OR_LATER
#if !UE_VERSION_OLDER_THAN(5, 2, 0)
#define UE_5_02_OR_LATER 1
#else
#define UE_5_02_OR_LATER 0
#endif
#endif

#ifndef UE_5_01_OR_LATER
#if !UE_VERSION_OLDER_THAN(5, 1, 0)
#define UE_5_01_OR_LATER 1
#else
#define UE_5_01_OR_LATER 0
#endif
#endif

#ifndef UE_5_00_OR_LATER
#if !UE_VERSION_OLDER_THAN(5, 0, 0)
#define UE_5_00_OR_LATER 1
#else
#define UE_5_00_OR_LATER 0
#endif
#endif

/**
 * Categories used for log output with this plugin.
 */
PLUGINBUILDER_API DECLARE_LOG_CATEGORY_EXTERN(LogPluginBuilder, Log, All);
