// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "testStyle.h"

class FtestCommands : public TCommands<FtestCommands>
{
public:

	FtestCommands()
		: TCommands<FtestCommands>(TEXT("test"), NSLOCTEXT("Contexts", "test", "test Plugin"), NAME_None, FtestStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};