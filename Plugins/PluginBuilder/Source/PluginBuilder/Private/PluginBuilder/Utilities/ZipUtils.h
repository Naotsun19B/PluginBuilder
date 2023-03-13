// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * A class that uses the IShellDispatch provided by the Windows shell to zip a folder.
	 */
	class PLUGINBUILDER_API FZipUtils
	{
	public:
		// Specify the directory path to be a zip file and
		// the output destination directory path to make the directory a zip file.
		static bool ZipUp(const FString& DirectoryPathToZipUp, const FString& OutputZipFilePath);
	};
}
