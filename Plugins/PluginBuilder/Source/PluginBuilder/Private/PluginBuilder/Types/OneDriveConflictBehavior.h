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
	// Overwrite the existing file.
	Replace,

	// Keep the existing file and upload the new file under a different name.
	Rename,

	// Abort the upload if a file with the same name already exists.
	Fail,
	
	// Skip the upload if the file already exists; retrieve a share URL for the existing file instead.
	Ignore,
};

// Converts EOneDriveConflictBehavior to the string expected by the Microsoft Graph API.
// Note: Ignore is handled before the API call and is never passed to the Graph API.
const TCHAR* LexToString(EOneDriveConflictBehavior Value);
