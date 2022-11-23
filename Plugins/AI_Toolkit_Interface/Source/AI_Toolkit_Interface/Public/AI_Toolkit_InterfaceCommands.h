// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "AI_Toolkit_InterfaceStyle.h"

class FAI_Toolkit_InterfaceCommands : public TCommands<FAI_Toolkit_InterfaceCommands>
{
public:

	FAI_Toolkit_InterfaceCommands()
		: TCommands<FAI_Toolkit_InterfaceCommands>(TEXT("AI_Toolkit_Interface"), NSLOCTEXT("Contexts", "AI_Toolkit_Interface", "AI_Toolkit_Interface Plugin"), NAME_None, FAI_Toolkit_InterfaceStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};