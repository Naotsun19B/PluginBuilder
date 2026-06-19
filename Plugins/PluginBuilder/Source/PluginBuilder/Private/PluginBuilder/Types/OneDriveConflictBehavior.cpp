// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/OneDriveConflictBehavior.h"

const TCHAR* LexToString(const EOneDriveConflictBehavior Value)
{
	switch (Value)
	{
	case EOneDriveConflictBehavior::Rename:
		return TEXT("rename");

	case EOneDriveConflictBehavior::Fail:
		return TEXT("fail");

	case EOneDriveConflictBehavior::Ignore:
		// Ignore is resolved before the upload call; this fallback is never reached.
		checkNoEntry();

	default:
		return TEXT("replace");
	}
}
