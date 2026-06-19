// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/OneDriveSettings.h"
#include "PluginBuilder/CloudStorages/OneDrive/OneDriveAuthenticator.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "UObject/ObjectMacros.h"

#define LOCTEXT_NAMESPACE "OneDriveSettings"

UOneDriveSettings::UOneDriveSettings()
	: ClientId(TEXT("70c7c67a-6d30-46ec-a33d-a3257d64c568"))
	, TokenExpiryTime(0)
{
}

void UOneDriveSettings::PostInitProperties()
{
	Super::PostInitProperties();

	if (ClientId.IsEmpty())
	{
		ClientId = TEXT("70c7c67a-6d30-46ec-a33d-a3257d64c568");
	}
}

FString UOneDriveSettings::GetSettingsName() const
{
	return TEXT("OneDrive");
}

void UOneDriveSettings::SignIn()
{
	if (ClientId.IsEmpty())
	{
		UE_LOG(LogPluginBuilder, Warning, TEXT("OneDrive: ClientId is not set. Please enter the Azure AD App Client ID in the settings."));
		return;
	}

	PluginBuilder::FOneDriveAuthenticator::StartAuthentication(
		ClientId, [this](bool bSuccess)
		{
			if (bSuccess)
			{
				UE_LOG(LogPluginBuilder, Log, TEXT("OneDrive: Signed in as %s."), *UserDisplayName);
				SaveConfig();

				FPropertyChangedEvent PropertyChangedEvent(nullptr, EPropertyChangeType::ValueSet);
				PostEditChangeProperty(PropertyChangedEvent);
			}
			else
			{
				UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Authentication failed."));
			}
		}
	);
}

void UOneDriveSettings::SignOut()
{
	ClearAuthentication();
	SaveConfig();
	UE_LOG(LogPluginBuilder, Log, TEXT("OneDrive: Signed out."));

	FPropertyChangedEvent PropertyChangedEvent(nullptr, EPropertyChangeType::ValueSet);
	PostEditChangeProperty(PropertyChangedEvent);
}

bool UOneDriveSettings::IsAuthenticated() const
{
	return (!AccessToken.IsEmpty() && !RefreshToken.IsEmpty());
}

void UOneDriveSettings::StoreTokens(
	const FString& InAccessToken,
	const FString& InRefreshToken,
	const int64 InExpiryTime,
	const FString& InUserDisplayName
)
{
	AccessToken = InAccessToken;
	RefreshToken = InRefreshToken;
	TokenExpiryTime = InExpiryTime;
	UserDisplayName = InUserDisplayName;
}

void UOneDriveSettings::ClearAuthentication()
{
	AccessToken = FString();
	RefreshToken = FString();
	TokenExpiryTime = 0;
	UserDisplayName = FString();
}

const FString& UOneDriveSettings::GetAccessToken() const
{
	return AccessToken;
}

const FString& UOneDriveSettings::GetRefreshToken() const
{
	return RefreshToken;
}

int64 UOneDriveSettings::GetTokenExpiryTime() const
{
	return TokenExpiryTime;
}

#undef LOCTEXT_NAMESPACE
