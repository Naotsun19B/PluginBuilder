// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommands.h"
#include "PluginBuilder/Utilities/PluginBuilderStyle.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/UIExtensions/ToolMenuExtender.h"
#include "PluginBuilder/Tasks/PackagePluginTask.h"

DEFINE_LOG_CATEGORY(LogPluginBuilder);

namespace PluginBuilder
{
	class FPluginBuilderModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};
	
	void FPluginBuilderModule::StartupModule()
	{
		// Register command actions.
		FPluginBuilderCommands::Register();
		FPluginBuilderCommands::Bind();

		// Register style set.
		FPluginBuilderStyle::Register();
		
		// Register settings.
		UPluginBuilderSettings::Register();

		// Register menu extension.
		FToolMenuExtender::Register();
	}

	void FPluginBuilderModule::ShutdownModule()
	{
		// Unregister menu extension.
		FToolMenuExtender::Unregister();
		
		// Unregister settings.
		UPluginBuilderSettings::Unregister();

		// Unregister style set.
		FPluginBuilderStyle::Unregister();
		
		// Unregister command actions.
		if (FPluginBuilderCommands::IsBound())
		{
			FPluginBuilderCommands::Unregister();
		}

		// Clean up working directory.
		FPackagePluginTask::CleanUp();
	}
}

IMPLEMENT_MODULE(PluginBuilder::FPluginBuilderModule, PluginBuilder)
