// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI_Toolkit_Interface.h"
#include "AI_Toolkit_InterfaceStyle.h"
#include "AI_Toolkit_InterfaceCommands.h"
#include "Editor.h"
#include "LevelEditor.h"
#include <Misc/FileHelper.h>
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName AI_Toolkit_InterfaceTabName("AI_Toolkit_Interface");

#define LOCTEXT_NAMESPACE "FAI_Toolkit_InterfaceModule"

/* getBatchContents creates the contents of the batch file for the creation process to write to the file. The comannds are built separately and are joined to return the full contents
*/
FString FAI_Toolkit_InterfaceModule::getBatchContents(FString Venv)
{
	auto cfg = FString::Printf(TEXT("for /f \"tokens=1,2 delims==\" %%%%a in (%s/aitoolkitinterface.ini) do (\r\nif %%%%a==toolDirectory set toolDirectory=%%%%b\r\nif %%%%a==previousVenv set previousVenv=%%%%b\r\nif %%%%a==currentOutputDirectory set currentOutputDirectory=%%%%b\r\nif %%%%a==scriptPath set scriptPath=%%%%b\r\nif %%%%a==imagePromptPath set imagePromptPath=%%%%b\r\n)\r\n"), *str_pluginDirectory);
	auto debug = FString::Printf(TEXT("echo %%toolDirectory%%\r\necho %%previousVenv%%\r\necho %%currentOutputDirectory%%\r\n"));
	auto innerCMD = FString::Printf(TEXT("\"%%scriptPath%%\" %%*"));
	auto cmd2 = FString::Printf(TEXT("%s\r\ncall conda.bat activate %%previousVenv%%\r\nfor /f \"delims=\" %%%%i in (\'python %s --outdir=\"%%currentOutputDirectory%%\"\') do set VARIABLE=%%%%i\r\necho %%VARIABLE%%\r\n"), *cfg, *innerCMD);
	return cmd2;
}

/* getBatchContents creates the contents of the .ini file for the creation process to write to the file. Each variable is built separately then joined to return the full variable list
*/

FString FAI_Toolkit_InterfaceModule::generateConfigContents()
{
	auto installDirectory = FString::Printf(TEXT("toolDirectory=%s"), *str_toolInstallDir);
	auto scriptPath = FString::Printf(TEXT("scriptPath=%s"), *str_scriptPath);
	auto imagePromptPath = FString::Printf(TEXT("imagePromptPath=%s"), *str_imgPromptPath);
	auto lastVenv = FString::Printf(TEXT("previousVenv=%s"), *str_currentVenv);
	auto outputDirectory = FString::Printf(TEXT("currentOutputDirectory=%s"), *str_outputDirectory);
	auto autoSave = FString::Printf(TEXT("autosaveSetting=%s"), bAutosave ? TEXT("true") : TEXT("false"));
	return FString::Printf(TEXT("%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n"), *installDirectory, *scriptPath, *lastVenv, *outputDirectory, *imagePromptPath,*autoSave);
}

/* CheckForPluginsFolder checks for the the required folder in the engine plugin directory, and creates it if not already available
*/
void FAI_Toolkit_InterfaceModule::CheckForPluginFolder()
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	bool folderExists = FileManager.DirectoryExists(*str_pluginDirectory);
	if (!folderExists)
	{
		FileManager.CreateDirectory(*str_pluginDirectory);
	}
}

/* generateBatchFile generates the batchfile required for communication between terminal, the tool, and Unreal.Default location is in the plugins folder, creates folder if not already existing.
*  Generating a new batchfile is required when the environment name is changed, which will overwrite the current file.
*/
FReply FAI_Toolkit_InterfaceModule::generateBatchFile()
{
	CheckForPluginFolder();
	FString plugin_BatchFile_path = FString::Printf(TEXT("%s/aitoolkitinterface.bat"), *str_pluginDirectory);
	FFileHelper::SaveStringToFile(getBatchContents(str_currentVenv), *plugin_BatchFile_path);

	str_batchFile_path = plugin_BatchFile_path;

	return FReply::Handled();
}

/* generateConfigFile writes the contents of the configuration file with the latest variable values to a file in the working plugin folder
*/
void FAI_Toolkit_InterfaceModule::generateConfigFile()
{
	CheckForPluginFolder();
	str_iniFile_path = FString::Printf(TEXT("%s/aitoolkitinterface.ini"), *str_pluginDirectory);
	FFileHelper::SaveStringToFile(generateConfigContents(), *str_iniFile_path);
}

/* SaveConfig saves the tool's configuration to disk and logs
*/
FReply FAI_Toolkit_InterfaceModule::SaveConfig()
{
	generateConfigFile();
	UE_LOG(LogTemp, Log, TEXT("Saved AI Toolkit Interface plugin configuration"));
	return FReply::Handled();
}

/* ReadConfigfile parses the values within the plugin configuration file and loads them into their respective variables
*/
FReply FAI_Toolkit_InterfaceModule::ReadConfigFile()
{
	FFileHelper::LoadFileToStringArray(iniOutput, *str_iniFile_path);
	for (auto line : iniOutput)
	{
		if (line.Contains(FString("=")))
		{
			int idx = line.Find(FString("="));
			auto wStr = line.LeftChop(line.Len() - idx);
			auto outVar = line.RightChop(idx + 1);
			if (wStr == "toolDirectory")
			{
				str_toolInstallDir = outVar;
				txt_toolDirectory->SetText(FText::FromString(str_toolInstallDir));
				bSetToolDirectory = true;
				continue;
			}
			if (wStr == "previousVenv")
			{
				str_currentVenv = outVar;
				txt_venvName->SetText(FText::FromString(FString::Printf(TEXT("Environment set to: %s"), *str_currentVenv)));
				etb_VenvName->SetText(FText::FromString(str_currentVenv));
				bSetVenvName = true;
				continue;
			}
			if (wStr == "currentOutputDirectory")
			{
				str_outputDirectory = outVar;
				txt_outputDirectory->SetText(FText::FromString(str_outputDirectory));
				bSetOutputDirectory = true;
				continue;
			}
			if (wStr == "scriptPath")
			{
				str_scriptPath = outVar;
				txt_scriptPath->SetText(FText::FromString(str_scriptPath));
				bSetScriptPath = true;
				continue;
			}
			if (wStr == "imagePromptPath")
			{
				str_imgPromptPath = outVar;
				txt_imgPrompt->SetText(FText::FromString(str_imgPromptPath));
				bSetImagePrompt = true;
				continue;
			}
			if (wStr == "autosaveSetting")
			{
				if (outVar.Contains("true"))
				{
					bAutosave = true;
					chk_Autosave->SetIsChecked(ECheckBoxState::Checked);
				}
				else
				{
					bAutosave = true;
					chk_Autosave->SetIsChecked(ECheckBoxState::Unchecked);
				}
				continue;
			}
		}
	}
	return FReply::Handled();
}

/*	GetToolkInstallDirectory() creates a directory prompt that relays back the address to the selected tool installation
*/
FReply FAI_Toolkit_InterfaceModule::GetToolkitInstallDirectory()
{
	FString out;
	IDesktopPlatform* dsk = FDesktopPlatformModule::Get();
	if (dsk)
	{
		uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
		dsk->OpenDirectoryDialog(nullptr, FString("Tool Path"), FString("C:"), out);
	}
	str_toolInstallDir = out;
	txt_toolDirectory->SetText(FText::FromString(str_toolInstallDir));
	bSetToolDirectory = true;

	if (bAutosave)
	{
		generateConfigFile();
	}

	return FReply::Handled();
}

/*	SetScriptPath() creates a file prompt that relays back the address to the selected script for execution
*/
FReply FAI_Toolkit_InterfaceModule::SetScriptPath()
{
	TArray<FString> out;
	IDesktopPlatform* dsk = FDesktopPlatformModule::Get();
	if (dsk)
	{
		uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
		dsk->OpenFileDialog(nullptr, FString("Image Path"), FString("C:"), FString(), FString("Scripts|*.py; *.bat;"), EFileDialogFlags::None, out);
	}
	str_scriptPath = out.IsEmpty() ? FString() : FPaths::ConvertRelativePathToFull(out[0]);
	txt_scriptPath->SetText(FText::FromString(FPaths::ConvertRelativePathToFull(str_scriptPath)));
	bSetScriptPath = str_scriptPath.IsEmpty() ? false : true;

	if (bAutosave)
	{
		generateConfigFile();
	}

	return FReply::Handled();
}

/*	SetImagePrompt() creates a file prompt that relays back the address to the selected image to use as a parameter for execution
*/
FReply FAI_Toolkit_InterfaceModule::SetImagePrompt()
{
	TArray<FString> out;
	IDesktopPlatform* dsk = FDesktopPlatformModule::Get();
	if (dsk)
	{
		uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
		dsk->OpenFileDialog(nullptr, FString("Image Path"), FString("C:"), FString(), FString("Photo Types|*.png; *.jpg; *.jpeg;"), EFileDialogFlags::None, out);
	}
	str_imgPromptPath = out.IsEmpty() ? FString() : FPaths::ConvertRelativePathToFull(out[0]);
	txt_imgPrompt->SetText(FText::FromString(str_imgPromptPath));
	bSetImagePrompt = str_imgPromptPath.IsEmpty() ? false : true;
	bSetTXT2IMGPrompt = bSetImagePrompt;

	if (bAutosave)
	{
		generateConfigFile();
	}

	return FReply::Handled();
}

void FAI_Toolkit_InterfaceModule::SetTextPrompt(const FText& InText, ETextCommit::Type CommitType)
{
	if ((CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus) && !InText.IsEmpty())
	{
		str_txt2img_prompt = InText.ToString();
		bSetTXT2IMGPrompt = true;
	}
	else if (InText.IsEmpty())
	{
		str_txt2img_prompt = FString();
		bSetTXT2IMGPrompt = false;
	}
}

void FAI_Toolkit_InterfaceModule::SetVenvName(const FText& InText, ETextCommit::Type CommitType)
{
	FString dispTxt{ FString() };
	if ((CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus) && !InText.IsEmpty())
	{
		str_currentVenv = InText.ToString();
		dispTxt = FString::Printf(TEXT("Environment set to: %s"), *str_currentVenv);
		bSetVenvName = true;
	}
	else if (InText.IsEmpty())
	{
		str_currentVenv = FString();
		dispTxt = FString::Printf(TEXT("Environment not set!"));
		bSetVenvName = false;
	}
	txt_venvName->SetText(FText::FromString(dispTxt));
	if (bAutosave)
	{
		generateConfigFile();
	}
}

FReply FAI_Toolkit_InterfaceModule::SetOutputDirectory()
{
	FString out;
	IDesktopPlatform* dsk = FDesktopPlatformModule::Get();
	if (dsk)
	{
		uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
		dsk->OpenDirectoryDialog(nullptr, FString("Set Output Directory"), FPaths::ProjectContentDir(), out);	//	Defaults to content directory
	}
	str_outputDirectory = out;
	char* cmd1 = TCHAR_TO_ANSI(*out); 
	txt_outputDirectory->SetText(FText::FromString(str_outputDirectory));
	bSetOutputDirectory = true;

	if (bAutosave)
	{
		generateConfigFile();
	}

	return FReply::Handled();
}

FReply FAI_Toolkit_InterfaceModule::InitiateScript()
{
	if ((bSetTXT2IMGPrompt || bSetImagePrompt) && bSetScriptPath)
	{
		generateConfigFile();
		UE_LOG(LogTemp, Log, TEXT("initiated txt2img process"));
		if (bSetImagePrompt)
		{
			FString prompt = FString::Printf(TEXT("--init-img=\"%s\""), *str_imgPromptPath);
			str_txt2img_prompt = FString::Printf(TEXT("%s %s"), *str_txt2img_prompt, *prompt);
		}
		void* inPipe{ nullptr };
		void* outPipe{ nullptr };
		FString plugin_BatchFile_dir = FPaths::ConvertRelativePathToFull(FPaths::EnginePluginsDir()).Append("AI_Toolkit_Interface");
		verify(FPlatformProcess::CreatePipe(inPipe, outPipe));
		auto proc = FPlatformProcess::CreateProc(*str_batchFile_path, *str_txt2img_prompt, false, false, false, nullptr, 0, *str_toolInstallDir, outPipe, inPipe);
		if (proc.IsValid())
		{
			FString output;
			while (FPlatformProcess::IsProcRunning(proc))
			{
				output += FPlatformProcess::ReadPipe(inPipe);
			}
			FPlatformProcess::ClosePipe(inPipe, outPipe);
			FPlatformProcess::CloseProc(proc);
			UE_LOG(LogTemp, Log, TEXT("%s"), *output);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Check params and configuration! Script Set? = %s / Set Text Prompt? = %s / Set Image Prompt? = %s"), bSetScriptPath, bSetTXT2IMGPrompt, bSetImagePrompt);
	}

	return FReply::Handled();
}

void FAI_Toolkit_InterfaceModule::UpdateAutosaveStatus(ECheckBoxState InState)
{
	switch (InState)
	{
	case ECheckBoxState::Unchecked:
		bAutosave = false;
		break;

	case ECheckBoxState::Checked:
		bAutosave = true;
		break;

	case ECheckBoxState::Undetermined:
		bAutosave = false;
		break;
	}
}

void FAI_Toolkit_InterfaceModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FAI_Toolkit_InterfaceStyle::Initialize();
	FAI_Toolkit_InterfaceStyle::ReloadTextures();

	FAI_Toolkit_InterfaceCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FAI_Toolkit_InterfaceCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FAI_Toolkit_InterfaceModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAI_Toolkit_InterfaceModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(AI_Toolkit_InterfaceTabName, FOnSpawnTab::CreateRaw(this, &FAI_Toolkit_InterfaceModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FAI_Toolkit_InterfaceTabTitle", "AI_Toolkit_Interface"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	CheckForPluginFolder();
	generateBatchFile();
}

void FAI_Toolkit_InterfaceModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FAI_Toolkit_InterfaceStyle::Shutdown();

	FAI_Toolkit_InterfaceCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(AI_Toolkit_InterfaceTabName);
}

TSharedRef<SDockTab> FAI_Toolkit_InterfaceModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText(LOCTEXT("WindowWidgetText", "Stable Diffusion Plugin"));
	FText sdInstall_Hint = FText(LOCTEXT("sdInstall_Hint", "Enter your Stable Diffusion installation path here."));

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
					.Padding(0, 10, 0, 0)
					[
						SNew(STextBlock)
						.Text(FText(LOCTEXT("ati_section_tool", "Tool Options")))
					]
				+ SVerticalBox::Slot()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SAssignNew(etb_VenvName, SEditableTextBox)
							.HintText(FText(LOCTEXT("venvText", "Enter python venv name here.")))
							.OnTextCommitted(FOnTextCommitted::CreateRaw(this, &FAI_Toolkit_InterfaceModule::SetVenvName))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1)
						.Padding(10, 0, 0, 0)
						[
							SAssignNew(txt_venvName, STextBlock)
							.Text(FText::FromString(str_currentVenv))
							.AutoWrapText(true)
						]
					]
				+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SAssignNew(btn_dirSearch, SButton)
							.OnClicked(FOnClicked::CreateRaw(this, &FAI_Toolkit_InterfaceModule::GetToolkitInstallDirectory))
							.Text(FText(LOCTEXT("btnText", "Search for Tool Directory")))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1)
						.Padding(10, 0, 0, 0)
						[
							SAssignNew(txt_toolDirectory, STextBlock)
							.Text(FText::FromString(str_toolInstallDir))
							.AutoWrapText(true)
						]
					]
				+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SAssignNew(btn_scriptSearch, SButton)
							.OnClicked(FOnClicked::CreateRaw(this, &FAI_Toolkit_InterfaceModule::SetScriptPath))
							.Text(FText(LOCTEXT("btn_script", "Search for Script Path")))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1)
						.Padding(10, 0, 0, 0)
						[
							SAssignNew(txt_scriptPath, STextBlock)
							.Text(FText::FromString(str_scriptPath))
							.AutoWrapText(true)
						]
					]
				+ SVerticalBox::Slot()
					.Padding(0, 20, 0, 0)
					[
						SNew(STextBlock)
						.Text(FText(LOCTEXT("ati_section_text2image", "Text to Image Configuration")))
					]
				+ SVerticalBox::Slot()
					[
						SAssignNew(etb_txt2imgPrompt, SEditableTextBox)
						.HintText(FText(LOCTEXT("hintTXT2IMG", "Text prompt for txt2img")))
						.OnTextCommitted(FOnTextCommitted::CreateRaw(this, &FAI_Toolkit_InterfaceModule::SetTextPrompt))
					]
				+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SButton)
							.OnClicked(FOnClicked::CreateRaw(this, &FAI_Toolkit_InterfaceModule::SetImagePrompt))
							.Text(FText(LOCTEXT("btn_imgPrompt", "Set Image Prompt")))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1)
						.Padding(10, 0, 0, 0)
						[
							SAssignNew(txt_imgPrompt, STextBlock)
							.Text(FText::FromString(str_imgPromptPath))
							.AutoWrapText(true)
						]
					]
				+ SVerticalBox::Slot()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SAssignNew(btn_outputDirectorySearch, SButton)
							.Text(FText(LOCTEXT("btn_outputDir", "Set Output Directory")))
							.OnClicked(FOnClicked::CreateRaw(this, &FAI_Toolkit_InterfaceModule::SetOutputDirectory))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1)
						.Padding(10, 0, 0, 0)
						[
							SAssignNew(txt_outputDirectory, STextBlock)
							.Text(FText::FromString(str_outputDirectory))
							.AutoWrapText(true)
						]
						
					]
				+ SVerticalBox::Slot()
					[
						SAssignNew(btn_txt2img, SButton)
						.Text(FText(LOCTEXT("btnTXT2IMG", "Generate image (txt2img)")))
						.OnClicked(FOnClicked::CreateRaw(this, &FAI_Toolkit_InterfaceModule::InitiateScript))
					]
				+ SVerticalBox::Slot()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SAssignNew(chk_Autosave, SCheckBox)
							.OnCheckStateChanged(FOnCheckStateChanged::CreateRaw(this, &FAI_Toolkit_InterfaceModule::UpdateAutosaveStatus))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1)
						.Padding(10, 0 ,0 ,0)
						[
							SNew(STextBlock)
							.Text(FText(LOCTEXT("txt_Autosave", "Enable Autosave Config")))
						]
					]
				+ SVerticalBox::Slot()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.AutoWidth()
						[
							SNew(SButton)
							.Text(FText(LOCTEXT("btn_LoadConfig", "Load Config")))
							.OnClicked(FOnClicked::CreateRaw(this, &FAI_Toolkit_InterfaceModule::ReadConfigFile))
						]
						+ SHorizontalBox::Slot()
							.AutoWidth()
						[
							SNew(SButton)
							.Text(FText(LOCTEXT("btn_SaveConfig", "Save Config")))
							.OnClicked(FOnClicked::CreateRaw(this, &FAI_Toolkit_InterfaceModule::SaveConfig))
						]
					]
			]
		];
}

void FAI_Toolkit_InterfaceModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(AI_Toolkit_InterfaceTabName);
}

void FAI_Toolkit_InterfaceModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FAI_Toolkit_InterfaceCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FAI_Toolkit_InterfaceCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAI_Toolkit_InterfaceModule, AI_Toolkit_Interface)