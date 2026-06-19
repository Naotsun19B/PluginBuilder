// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/Types/OneDriveAuthenticationActions.h"
#include "OneDriveSettings.generated.h"

/**
 * Editor preferences for OneDrive integration.
 * Stores authentication tokens and upload rules.
 */
UCLASS(GlobalUserConfig)
class PLUGINBUILDER_API UOneDriveSettings : public UPluginBuilderSettings
{
	GENERATED_BODY()

public:
	// This class is read-only from outside; use the setter methods for mutable access.
	using UReference = const UOneDriveSettings&;

public:
	// The Client ID of the Azure AD app used for OneDrive authentication.
	// Register a free app at https://portal.azure.com to obtain this value.
	UPROPERTY(EditAnywhere, Config, Category = "Authentication")
	FString ClientId;

	// Displays Sign In / Sign Out buttons for OneDrive authentication.
	UPROPERTY(EditAnywhere, Category = "Authentication")
	FOneDriveAuthenticationActions AuthenticationActions;

	// The display name of the signed-in Microsoft account (read-only).
	UPROPERTY(VisibleAnywhere, Config, Category = "Authentication", meta = (NoResetToDefault))
	FString UserDisplayName;

	// The base folder path inside OneDrive where plugins will be uploaded.
	// Leave empty to upload directly under My files (e.g. root/PluginName/...).
	// When set, files are placed under BaseFolderPath/PluginName/...
	UPROPERTY(EditAnywhere, Config, Category = "Upload")
	FString BaseFolderPath;

public:
	// Constructor.
	UOneDriveSettings();

	// UObject interface.
	virtual void PostInitProperties() override;
	// End of UObject interface.

	// UPluginBuilderSettings interface.
	virtual FString GetSettingsName() const override;
	// End of UPluginBuilderSettings interface.

	// Starts the OAuth2 authentication flow and opens the browser.
	void SignIn();

	// Clears the stored authentication tokens and signs out.
	void SignOut();

	// Returns whether the user currently has valid authentication tokens.
	bool IsAuthenticated() const;

	// Stores tokens and display name after a successful authentication.
	void StoreTokens(
		const FString& InAccessToken,
		const FString& InRefreshToken,
		int64 InExpiryTime,
		const FString& InUserDisplayName
	);

	// Clears all stored authentication data.
	void ClearAuthentication();

	// Returns the stored access token.
	const FString& GetAccessToken() const;

	// Returns the stored refresh token.
	const FString& GetRefreshToken() const;

	// Returns the token expiry time as a Unix timestamp.
	int64 GetTokenExpiryTime() const;

private:
	// Stored access token — not shown in the editor UI.
	UPROPERTY(Config)
	FString AccessToken;

	// Stored refresh token — not shown in the editor UI.
	UPROPERTY(Config)
	FString RefreshToken;

	// Token expiry as a Unix timestamp (seconds since epoch).
	UPROPERTY(Config)
	int64 TokenExpiryTime;
};
