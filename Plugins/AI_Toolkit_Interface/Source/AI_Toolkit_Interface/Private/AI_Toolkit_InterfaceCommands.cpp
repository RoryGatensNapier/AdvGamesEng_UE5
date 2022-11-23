// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI_Toolkit_InterfaceCommands.h"

#define LOCTEXT_NAMESPACE "FAI_Toolkit_InterfaceModule"

void FAI_Toolkit_InterfaceCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "AI_Toolkit_Interface", "Bring up AI_Toolkit_Interface window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
