// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Slate.h"

class FToolBarBuilder;
class FMenuBuilder;

class FtestModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	FReply DirFunc();
	FReply InitiateTXT2IMG();
	void SetTXT2IMGPrompt(const FText&, ETextCommit::Type);
	
private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	//	Plugin Commands
	TSharedPtr<class FUICommandList> PluginCommands;
	
	// Window slate elements
	TSharedPtr<SButton> btn_dirSearch;	//	Button to set directory for Stable Diffusion installation
	TSharedPtr<STextBlock> txt_toolDirectory;	//	Directory string display
	TSharedPtr<SEditableTextBox> etb_txt2imgPrompt;	//	Text input box for txt2img prompt
	TSharedPtr<SButton> txt2img_btn;	// Button to initiate txt2img with prompt

	// Install directory string
	FString installDir{FString("Directory not set!")};
	FString str_txt2img_prompt{ FString() };

private:
	//	True when directory for Stable Diffusion has been set
	bool bSetDirectory{ false };
	bool bSetTXT2IMGPrompt{ false };
};
