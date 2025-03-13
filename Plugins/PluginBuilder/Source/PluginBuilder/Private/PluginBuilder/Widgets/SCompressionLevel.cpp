// Copyright 2022-2025 Naotsun. All Rights Reserved.

#include "PluginBuilder/Widgets/SCompressionLevel.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
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
					return UPluginBuilderSettings::Get().bZipUp;
				}
			)
			.Value_Lambda(
				[]() -> uint8
				{
					return UPluginBuilderSettings::Get().CompressionLevel;
				}
			)
			.OnValueChanged_Lambda(
				[](const uint8 Value)
				{
					UPluginBuilderSettings::ModifyProperties(
						[&](UPluginBuilderSettings& Settings)
						{
							Settings.CompressionLevel = Value;
						}
					);
				}
			)
		);
	}

	FToolMenuEntry SCompressionLevel::MakeToolMenuWidget()
	{
		const FName VariableName = GET_MEMBER_NAME_CHECKED(UPluginBuilderSettings, CompressionLevel);

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
