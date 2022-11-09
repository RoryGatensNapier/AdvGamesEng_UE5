// Copyright Epic Games, Inc. All Rights Reserved.

#include "test.h"
#include "testStyle.h"
#include "testCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"

static const FName testTabName("test");

#define LOCTEXT_NAMESPACE "FtestModule"

FReply FtestModule::DirFunc()
{
	FString out;
	IDesktopPlatform* dsk = FDesktopPlatformModule::Get();
	if (dsk)
	{
		uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
		dsk->OpenDirectoryDialog(nullptr, FString("Stable Diffusion Path"), FString("C:"), out);
	}
	installDir = out;
	dirText->SetText(FText::FromString(installDir));
	bSetDirectory = true;
	return FReply::Handled();
}

void FtestModule::SetTXT2IMGPrompt(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus)
	{
		str_txt2img_prompt = InText.ToString();
		bSetTXT2IMGPrompt = true;
	}
}

FReply FtestModule::InitiateTXT2IMG()
{
	if (bSetTXT2IMGPrompt)
	{
		UE_LOG(LogTemp, Log, TEXT("initiated txt2img process"));
	}
}

void FtestModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FtestStyle::Initialize();
	FtestStyle::ReloadTextures();

	FtestCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FtestCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FtestModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FtestModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(testTabName, FOnSpawnTab::CreateRaw(this, &FtestModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FtestTabTitle", "test"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FtestModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FtestStyle::Shutdown();

	FtestCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(testTabName);
}

TSharedRef<SDockTab> FtestModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
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
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SAssignNew(dirSearch, SButton)
					.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				.OnClicked(FOnClicked::CreateRaw(this, &FtestModule::DirFunc))
				.Text(FText(LOCTEXT("btnText", "Search for Directory")))
				]
				
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(dirText, STextBlock)
					.Text(FText::FromString(installDir))
				]

				+ SVerticalBox::Slot()
				[
					SAssignNew(txt2img_prompt, SEditableTextBox)
					.HintText(FText(LOCTEXT("hintTXT2IMG","Text prompt for txt2img")))
					.OnTextCommitted(FOnTextCommitted::CreateRaw(this, &FtestModule::SetTXT2IMGPrompt))
				]

				+ SVerticalBox::Slot()
				[
					SAssignNew(txt2img_btn, SButton)
					.Text(FText(LOCTEXT("btnTXT2IMG", "Generate image (txt2img)")))
					.OnClicked(FOnClicked::CreateRaw(this, &FtestModule::InitiateTXT2IMG))
				]
			]
		];
}

void FtestModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(testTabName);
}

void FtestModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FtestCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FtestCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FtestModule, test)