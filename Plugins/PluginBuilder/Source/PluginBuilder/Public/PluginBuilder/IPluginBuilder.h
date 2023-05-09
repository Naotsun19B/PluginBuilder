// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PluginBuilder/Types/PackagePluginParams.h"

namespace PluginBuilder
{
	/**
	 * The public interface to the GraphMinimap module.
	 */
	class IPluginBuilder : public IModuleInterface
	{
	public:
		// The name of the module for this plugin.
		PLUGINBUILDER_API static const FName ModuleName;
		
	public:
		// Returns singleton instance, loading the module on demand if needed.
		static IPluginBuilder& Get()
		{
			return FModuleManager::LoadModuleChecked<IPluginBuilder>(ModuleName);
		}

		// Returns whether the module is loaded and ready to use.
		static bool IsAvailable()
		{
			return FModuleManager::Get().IsModuleLoaded(ModuleName);
		}

		// Creates and starts a task to specify a parameters and package the plugin.
		// If you don't specify parameters, it will be created from the values set in the editor preferences.
		// Returns whether the package plugin task has started.
		virtual bool StartPackagePluginTask(const TOptional<FPackagePluginParams>& InParams = {}) = 0;

		// Returns whether package processing is being done.
		virtual bool IsPackagePluginTaskRunning() = 0;
	};
}
