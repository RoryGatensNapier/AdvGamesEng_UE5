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

	FReply GetToolkitInstallDirectory();
	FReply SetScriptPath();
	FReply SetOutputDirectory();
	FReply SetImagePrompt();

	FReply InitiateScript();
	void SetTextPrompt(const FText&, ETextCommit::Type);
	
	void SetVenvName(const FText&, ETextCommit::Type);
	
	FString getBatchContents(FString Venv);
	
	FReply generateBatchFile();

	void generateConfigFile();
	FString generateConfigContents();
	FReply ReadConfigFile();
	FReply SaveConfig();
	void UpdateAutosaveStatus(ECheckBoxState InState);

	void CheckForPluginFolder();
	
private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	// Window slate elements
	TSharedPtr<SButton> btn_dirSearch;	//	Button to set directory for Stable Diffusion installation
	TSharedPtr<SButton> btn_scriptSearch;	//	Button to set directory for Stable Diffusion installation
	TSharedPtr<SButton> btn_outputDirectorySearch;	//	Button to set output directory for resultant samples
	TSharedPtr<SButton> btn_txt2img;	// Button to initiate txt2img with prompt
	TSharedPtr<SButton> btn_generateBatch;	// Button to initiate txt2img with prompt


	TSharedPtr<STextBlock> txt_toolDirectory;	//	Directory string display
	TSharedPtr<STextBlock> txt_scriptPath;	//	Directory string display
	TSharedPtr<STextBlock> txt_imgPrompt;	//	Directory string display
	TSharedPtr<STextBlock> txt_outputDirectory;	//	Directory string display
	TSharedPtr<STextBlock> txt_venvName;	//	Venv string display

	TSharedPtr<SEditableTextBox> etb_VenvName;	//	Text input box for current venv name
	TSharedPtr<SEditableTextBox> etb_txt2imgPrompt;	//	Text input box for txt2img prompt

	TSharedPtr<SCheckBox> chk_Autosave;	//	Autosave checkbox


	FString str_pluginDirectory{ FPaths::ConvertRelativePathToFull(FPaths::EnginePluginsDir()).Append("AI_Toolkit_Interface") };	//	Default plugin directory to Engine
	FString str_toolInstallDir{ FString("Directory not set!") };	// Install directory string
	FString str_txt2img_prompt{ FString() };	//	Text 2 image prompt string, passed to bat
	FString str_currentVenv{ FString("Venv name not set!")};	//	Current Venv name string
	FString str_batchFile_path{ FString::Printf(TEXT("%s/aitoolkitinterface.bat"), *str_pluginDirectory) };	//	Path to batchfile
	FString str_iniFile_path{ FString::Printf(TEXT("%s/aitoolkitinterface.ini"), *str_pluginDirectory) };
	FString str_outputDirectory{ FString("Output directory not set!") };	// Path to output directory
	FString str_scriptPath{ FString("Script Folder not set!") };
	FString str_imgPromptPath{ FString("Image Prompt not set!") };

	TArray<FString> iniOutput;

private:

	bool bSetToolDirectory{ false };	//	True when directory for Stable Diffusion has been set
	bool bSetScriptPath{ false };	//	True when directory for Stable Diffusion has been set
	bool bSetOutputDirectory{ false };	//	True when the output directory has been set
	bool bSetTXT2IMGPrompt{ false };	//	True when valid text is set in the txt2img prompt text box
	bool bSetVenvName{ false };	//	True when Venv name has been set in the environmnet name box
	bool bSetImagePrompt{ false }; //	True when an image file has been selected for a prompt
	bool bAutosave{ true };	// Autosave setting
};
