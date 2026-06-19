// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OneDriveConflictBehavior.generated.h"

/**
 * Determines how a filename conflict is resolved when uploading to OneDrive.
 */
UENUM()
enum class EOneDriveConflictBehavior : uint8
{
	Replace	UMETA(DisplayName = "Replace"),
	Rename	UMETA(DisplayName = "Rename"),
	Fail	UMETA(DisplayName = "Fail"),
};

// Converts EOneDriveConflictBehavior to the string expected by the Microsoft Graph API.
const TCHAR* LexToString(EOneDriveConflictBehavior Value);
