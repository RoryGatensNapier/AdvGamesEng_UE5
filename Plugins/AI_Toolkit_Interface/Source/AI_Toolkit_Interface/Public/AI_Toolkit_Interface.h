// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"

class FToolBarBuilder;
class FMenuBuilder;

class FAI_Toolkit_InterfaceModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	FReply getUserDirectory();
	FReply InitiateTXT2IMG();
	void SetTXT2IMGPrompt(const FText&, ETextCommit::Type);
	void SetVenvName(const FText&, ETextCommit::Type);
	FString getBatchContents(FString Venv);
	FReply generateEnvironmentBatchFile();
	
private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	// Window slate elements
	TSharedPtr<SButton> dirSearch;	//	Button to set directory for Stable Diffusion installation
	TSharedPtr<STextBlock> dirText;	//	Directory string display
	TSharedPtr<STextBlock> VenvText;	//	Directory string display
	TSharedPtr<SEditableTextBox> txt2img_prompt;	//	Text input box for txt2img prompt
	TSharedPtr<SButton> txt2img_btn;	// Button to initiate txt2img with prompt
	TSharedPtr<SButton> genBatBtn;	// Button to initiate txt2img with prompt

	// Install directory string
	FString str_toolInstallDir{ FString("Directory not set!") };
	FString str_txt2img_prompt{ FString() };
	FString str_currentVenv{ FString("Venv name not set!")};
	FString str_batchFile_path{ FString() };

private:
	//	True when directory for Stable Diffusion has been set
	bool bSetDirectory{ false };
	bool bSetTXT2IMGPrompt{ false };
	bool bSetVenvName{ false };

};
