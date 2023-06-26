// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Version.h"

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
#ifndef ENGINE_VERSION_NUMBER
#define ENGINE_VERSION_NUMBER(MajorVersion, MinorVersion) (MajorVersion * 100 + MinorVersion)
#endif
#ifndef COMPARE_ENGINE_VERSION
#define COMPARE_ENGINE_VERSION(MajorVersion, MinorVersion) ENGINE_VERSION_NUMBER(ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION) >= ENGINE_VERSION_NUMBER(MajorVersion, MinorVersion)
#endif

#ifndef UE_5_01_OR_LATER
#if COMPARE_ENGINE_VERSION(5, 1)
#define UE_5_01_OR_LATER 1
#else
#define UE_5_01_OR_LATER 0
#endif
#endif

#ifndef UE_5_00_OR_LATER
#if COMPARE_ENGINE_VERSION(5, 0)
#define UE_5_00_OR_LATER 1
#else
#define UE_5_00_OR_LATER 0
#endif
#endif

/**
 * Categories used for log output with this plugin.
 */
PLUGINBUILDER_API DECLARE_LOG_CATEGORY_EXTERN(LogPluginBuilder, Log, All);
