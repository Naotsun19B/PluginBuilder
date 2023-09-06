// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderStyle.h"
#include "Interfaces/IPluginManager.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/CoreStyle.h"

namespace PluginBuilder
{
#if !UE_5_00_OR_LATER
	namespace CoreStyleConstants
	{
		static const FVector2D Icon16x16(16.f, 16.f);
	}
#endif
	
	FPluginBuilderStyle::FPluginBuilderStyle()
		: FSlateStyleSet(TEXT("PluginBuilderStyle"))
	{
	}

	void FPluginBuilderStyle::RegisterInternal()
	{
		const TArray<TSharedRef<IPlugin>> Plugins = IPluginManager::Get().GetDiscoveredPlugins();
		for (const auto& Plugin : Plugins)
		{
			const FName IconName = GetPropertyName(Plugin->GetFriendlyName());
			FString IconPath = FPaths::ConvertRelativePathToFull(
				Plugin->GetBaseDir() / TEXT("Resources") / TEXT("Icon128.png")
			);

			const TSharedPtr<IPlugin> PluginBrowser = IPluginManager::Get().FindPlugin(TEXT("PluginBrowser"));
			if (PluginBrowser.IsValid() && !FPaths::FileExists(IconPath))
			{
				IconPath = FPaths::ConvertRelativePathToFull(
					PluginBrowser->GetBaseDir() / TEXT("Resources") / TEXT("DefaultIcon128.png")
				);
			}
			
			Set(
				IconName,
				new FSlateImageBrush(IconPath, CoreStyleConstants::Icon16x16)
			);
		}
	}

	void FPluginBuilderStyle::Register()
	{
		Instance = MakeShared<FPluginBuilderStyle>();
		Instance->RegisterInternal();
		FSlateStyleRegistry::RegisterSlateStyle(*Instance);
	}

	void FPluginBuilderStyle::Unregister()
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*Instance);
		Instance.Reset();
	}

	const ISlateStyle& FPluginBuilderStyle::Get()
	{
		check(Instance.IsValid()); // Don't call before Register is called or after Unregister is called.
		return *Instance.Get();
	}

	FName FPluginBuilderStyle::GetPropertyName(const FString& PluginFriendlyName)
	{
		return *FString::Printf(TEXT("%s.Icons.%s"), *Global::PluginName.ToString(), *PluginFriendlyName);
	}

	TSharedPtr<FPluginBuilderStyle> FPluginBuilderStyle::Instance = nullptr;
}
