// Copyright 2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "CommandActions/PluginBuilderCommands.h"
#include "Modules/ModuleManager.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderMenuExtension.h"

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
		
		// Register settings.
		UPluginBuilderSettings::Register();

		// Register menu extension.
		FPluginBuilderMenuExtension::Register();
	}

	void FPluginBuilderModule::ShutdownModule()
	{
		// Unregister menu extension.
		FPluginBuilderMenuExtension::Unregister();
		
		// Unregister settings.
		UPluginBuilderSettings::Unregister();

		// Unregister command actions.
		if (FPluginBuilderCommands::IsBound())
		{
			FPluginBuilderCommands::Unregister();
		}
	}
}

IMPLEMENT_MODULE(PluginBuilder::FPluginBuilderModule, PluginBuilder)
