// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UToolMenu;

namespace PluginBuilder
{
	/**
	 * A class that extends the level editor menu.
	 */
	class PLUGINBUILDER_API FToolMenuExtender
	{
	public:
		// The proper name of the extension point added by this menu extension.
		static const FName FilePluginSectionName;
		static const FName PackagePluginSubMenuName;
		static const FName PackagePluginSectionName;
		static const FName BuildConfigurationsSubMenuName;
		static const FName BuildTargetsSubMenuName;
		static const FName VersionsAndPlatformsSectionName;
		static const FName EngineVersionsSubMenuName;
		static const FName TargetPlatformsSubMenuName;
		static const FName BuildOptionsSectionName;
		static const FName ZipUpOptionsSectionName;
		static const FName EngineVersionPresetSectionName;
		
	public:
		// Registers-Unregisters the menu extension.
		static void Register();
		static void Unregister();

	private:
		// Returns the extension point of the mainframe's file menu.
		static UToolMenu* GetMenuExtensionPoint();
		
		// Functions called when sub menus are generated.
		static void OnExtendPackagePluginSubMenu(UToolMenu* ToolMenu);
		static void OnExtendBuildConfigurationsSubMenu(UToolMenu* ToolMenu);
		static void OnExtendEngineVersionsSubMenu(UToolMenu* ToolMenu);
		static void OnExtendTargetPlatformsSubMenu(UToolMenu* ToolMenu);
		static void OnExtendBuildTargetsSubMenu(UToolMenu* ToolMenu);
	};
}
