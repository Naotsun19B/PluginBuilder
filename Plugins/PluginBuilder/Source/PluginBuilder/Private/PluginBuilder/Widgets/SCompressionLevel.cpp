// Copyright 2022-2025 Naotsun. All Rights Reserved.

#include "PluginBuilder/Widgets/SCompressionLevel.h"
#include "PluginBuilder/Utilities/PluginBuilderBuildConfigurationSettings.h"
#include "PluginBuilder/PluginBuilderGlobals.h"

namespace PluginBuilder
{
	void SCompressionLevel::Construct(const FArguments& InArgs)
	{
		SNumericEntryBox::Construct(
			SNumericEntryBox::FArguments()
			.AllowSpin(true)
			.MinValue(0)
			.MaxValue(9)
			.MinSliderValue(0)
			.MaxSliderValue(9)
			.IsEnabled_Lambda(
				[]() -> bool
				{
					const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
					return Settings.bZipUp;
				}
			)
			.Value_Lambda(
				[]() -> uint8
				{
					const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
					return Settings.CompressionLevel;
				}
			)
			.OnValueChanged_Lambda(
				[](const uint8 Value)
				{
					auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
					Settings.CompressionLevel = Value;
				}
			)
		);
	}

	FToolMenuEntry SCompressionLevel::MakeToolMenuWidget()
	{
		const FName VariableName = GET_MEMBER_NAME_CHECKED(UPluginBuilderBuildConfigurationSettings, CompressionLevel);

#if UE_5_02_OR_LATER
		FText VariableTooltip;
		for (const auto* ByteProperty : TFieldRange<FByteProperty>(UPluginBuilderSettings::StaticClass()))
		{
			if (ByteProperty == nullptr)
			{
				continue;
			}

			if (ByteProperty->GetFName() != VariableName)
			{
				continue;
			}

			VariableTooltip = ByteProperty->GetToolTipText();
			break;
		}
#endif

		return FToolMenuEntry::InitWidget(
			VariableName,
			SNew(SCompressionLevel),
			FText::FromName(VariableName)
#if UE_5_02_OR_LATER
			,
			false,
			true,
			false,
			VariableTooltip
#endif
		);
	}
	
}
