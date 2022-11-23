// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI_Toolkit_Interface.h"
#include "AI_Toolkit_InterfaceStyle.h"
#include "AI_Toolkit_InterfaceCommands.h"
#include "LevelEditor.h"
#include <Misc/FileHelper.h>
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName AI_Toolkit_InterfaceTabName("AI_Toolkit_Interface");

#define LOCTEXT_NAMESPACE "FAI_Toolkit_InterfaceModule"

FString getBatchContents(FString Venv) 
{
	auto cmd = FString("call conda.bat activate ").Append(Venv);
	auto cmd2 = FString::Printf(TEXT("call conda.bat activate %s\r\nfor /f \"delims=\" %%%%i in ('python --version') do set VARIABLE=%%%%i\r\necho %%VARIABLE%%\r\ncall conda deactivate\r\n"), *Venv);
	return cmd2;
}

FReply FAI_Toolkit_InterfaceModule::getUserDirectory()
{
	FString out;
	IDesktopPlatform* dsk = FDesktopPlatformModule::Get();
	if (dsk)
	{
		uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
		dsk->OpenDirectoryDialog(nullptr, FString("Stable Diffusion Path"), FString("C:"), out);
	}
	str_toolInstallDir = out;
	char* cmd1 = TCHAR_TO_ANSI(*out);
	dirText->SetText(FText::FromString(str_toolInstallDir));
	bSetDirectory = true;
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString plugin_BatchFile_dir = FPaths::ConvertRelativePathToFull(FPaths::EnginePluginsDir()).Append("AI_Toolkit_Interface");
	bool folderExists = FileManager.DirectoryExists(*plugin_BatchFile_dir);
	if (!folderExists)
	{
		FileManager.CreateDirectory(*plugin_BatchFile_dir);
	}
	FString plugin_BatchFile_path = plugin_BatchFile_dir.Append("/aitoolkitinterface.bat");
	bool batchExists = FileManager.FileExists(*plugin_BatchFile_path);
	if (!batchExists)
	{
		FFileHelper::SaveStringToFile(getBatchContents(str_currentVenv), *plugin_BatchFile_path);
		/*auto h = FileManager.OpenWrite(*plugin_BatchFile_path);
		h->~IFileHandle();*/
	}
	str_batchFile_path = plugin_BatchFile_path;

	return FReply::Handled();
}

void FAI_Toolkit_InterfaceModule::SetTXT2IMGPrompt(const FText& InText, ETextCommit::Type CommitType)
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
	if ((CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus) && !InText.IsEmpty())
	{
		str_currentVenv = InText.ToString();
		bSetVenvName = true;
	}
	else if (InText.IsEmpty())
	{
		str_currentVenv = FString("Venv name not set!");
		bSetVenvName = false;
	}
}

FReply FAI_Toolkit_InterfaceModule::InitiateTXT2IMG()
{
	if (bSetTXT2IMGPrompt)
	{
		UE_LOG(LogTemp, Log, TEXT("initiated txt2img process"));
		auto command = FString::Printf(TEXT("cmd.exe"));
		auto params = FString::Printf(TEXT("aitoolkitinterface.bat"));
		//auto params = FString::Printf(TEXT("run -n stylegan3 gen_images.py --outdir=out --trunc=1 --seeds=2 --network=https://api.ngc.nvidia.com/v2/models/nvidia/research/stylegan3/versions/1/files/stylegan3-r-afhqv2-512x512.pkl"));
		void* testIn{ nullptr };
		void* testOut{ nullptr };
		FString plugin_BatchFile_dir = FPaths::ConvertRelativePathToFull(FPaths::EnginePluginsDir()).Append("AI_Toolkit_Interface");
		verify(FPlatformProcess::CreatePipe(testIn, testOut));
		auto proc = FPlatformProcess::CreateProc(*str_batchFile_path, /**params*/nullptr, false, false, false, nullptr, 0, /**plugin_BatchFile_dir*/nullptr, testOut, testIn);
		if (proc.IsValid())
		{
			FString output;
			while (FPlatformProcess::IsProcRunning(proc))
			{
				output += FPlatformProcess::ReadPipe(testIn);
			}
			///FPlatformProcess::ClosePipe(testIn, testOut);
			//FPlatformProcess::CloseProc(proc);
			UE_LOG(LogTemp, Log, TEXT("%s"), *output);
		}
	}

	return FReply::Handled();
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
					[
						SAssignNew(txt2img_prompt, SEditableTextBox)
						.HintText(FText(LOCTEXT("venvText", "Enter python venv name here.")))
						.OnTextCommitted(FOnTextCommitted::CreateRaw(this, &FAI_Toolkit_InterfaceModule::SetVenvName))
					]
				+ SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Top)
					[
						SAssignNew(dirSearch, SButton)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Top)
						.OnClicked(FOnClicked::CreateRaw(this, &FAI_Toolkit_InterfaceModule::getUserDirectory))
						.Text(FText(LOCTEXT("btnText", "Search for Tool Directory")))
					]
				+ SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SAssignNew(dirText, STextBlock)
						.Text(FText::FromString(str_toolInstallDir))
					]

				+ SVerticalBox::Slot()
					[
						SAssignNew(txt2img_prompt, SEditableTextBox)
						.HintText(FText(LOCTEXT("hintTXT2IMG", "Text prompt for txt2img")))
						.OnTextCommitted(FOnTextCommitted::CreateRaw(this, &FAI_Toolkit_InterfaceModule::SetTXT2IMGPrompt))
					]
				+ SVerticalBox::Slot()
					[
						SAssignNew(txt2img_btn, SButton)
						.Text(FText(LOCTEXT("btnTXT2IMG", "Generate image (txt2img)")))
						.OnClicked(FOnClicked::CreateRaw(this, &FAI_Toolkit_InterfaceModule::InitiateTXT2IMG))
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