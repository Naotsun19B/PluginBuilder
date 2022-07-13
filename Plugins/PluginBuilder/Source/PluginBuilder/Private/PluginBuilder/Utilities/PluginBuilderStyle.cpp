// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Textures/SlateIcon.h"

namespace PluginBuilder
{
	namespace IconSize
	{
		static const FVector2D Icon16x16(16.0f, 16.0f);
	}
	
	FPluginBuilderStyle::FPluginBuilderStyle()
		: FSlateStyleSet(TEXT("PluginBuilderStyle"))
	{
	}
	
	void FPluginBuilderStyle::Register()
	{
		Instance = MakeShared<FPluginBuilderStyle>();

		const TArray<TSharedRef<IPlugin>> Plugins = IPluginManager::Get().GetDiscoveredPlugins();
		for (const auto& Plugin : Plugins)
		{
			const FName IconName = GetPropertyName(Plugin->GetFriendlyName());
			const FString IconPath = FPaths::ConvertRelativePathToFull(
				Plugin->GetBaseDir() / TEXT("Resources") / TEXT("Icon128.png")
			);

			Instance->Set(
				IconName,
				new FSlateImageBrush(IconPath, IconSize::Icon16x16)
			);
		}
		
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

	FName FPluginBuilderStyle::GetPropertyName(const FString& PluginName)
	{
		return *FString::Printf(TEXT("PluginBuilder.Icons.%s"), *PluginName);
	}

	TSharedPtr<FPluginBuilderStyle> FPluginBuilderStyle::Instance = nullptr;
}
