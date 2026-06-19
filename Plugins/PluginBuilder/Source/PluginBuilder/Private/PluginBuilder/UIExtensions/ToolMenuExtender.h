// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/Types/PlatformsBase.h"

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
		static const FName BuildConfigurationSubMenuName;
		static const FName BuildTargetSubMenuName;
		static const FName ZipUpConfigurationSubMenuName;
		static const FName CloudStorageConfigurationSubMenuName;
		static const FName VersionsAndPlatformsSectionName;
		static const FName EngineVersionsSubMenuName;
		static const FName HostPlatformsSubMenuName;
		static const FName TargetPlatformsSubMenuName;
		static const FName BuildOptionsSectionName;
		static const FName ZipUpOptionsSectionName;
		static const FName CloudStorageOptionsSectionName;
		static const FName ConflictBehaviorSubMenuName;
		static const FName EngineVersionPresetSectionName;
		static const FName IndividualStepsSectionName;
		
	public:
		// Registers-Unregisters the menu extension.
		static void Register();
		static void Unregister();

	private:
		// Returns the extension point of the mainframe's file menu.
		static UToolMenu* GetMenuExtensionPoint();
		
		// Functions called when sub menus are generated.
		static void OnExtendPackagePluginSubMenu(UToolMenu* ToolMenu);
		static void OnExtendBuildConfigurationSubMenu(UToolMenu* ToolMenu);
		static void OnExtendBuildTargetSubMenu(UToolMenu* ToolMenu);
		static void OnExtendZipUpConfigurationSubMenu(UToolMenu* ToolMenu);
		static void OnExtendCloudStorageConfigurationSubMenu(UToolMenu* ToolMenu);
		static void OnExtendConflictBehaviorSubMenu(UToolMenu* ToolMenu);
		static void OnExtendEngineVersionsSubMenu(UToolMenu* ToolMenu);
		static void OnExtendHostPlatformsSubMenu(UToolMenu* ToolMenu);
		static void OnExtendTargetPlatformsSubMenu(UToolMenu* ToolMenu);
	};
}
