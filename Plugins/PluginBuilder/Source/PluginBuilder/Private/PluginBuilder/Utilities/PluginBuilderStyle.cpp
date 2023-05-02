// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderStyle.h"
#include "Interfaces/IPluginManager.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/CoreStyle.h"

namespace PluginBuilder
{
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
			const FString IconPath = FPaths::ConvertRelativePathToFull(
				Plugin->GetBaseDir() / TEXT("Resources") / TEXT("Icon128.png")
			);

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
		return *FString::Printf(TEXT("%s.Icons.%s"), *PluginName.ToString(), *PluginFriendlyName);
	}

	TSharedPtr<FPluginBuilderStyle> FPluginBuilderStyle::Instance = nullptr;
}
