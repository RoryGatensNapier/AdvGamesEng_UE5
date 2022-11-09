// Copyright Epic Games, Inc. All Rights Reserved.

#include "testCommands.h"

#define LOCTEXT_NAMESPACE "FtestModule"

void FtestCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "test", "Bring up test window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
