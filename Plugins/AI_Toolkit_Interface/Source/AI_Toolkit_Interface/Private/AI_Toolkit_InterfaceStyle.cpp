// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI_Toolkit_InterfaceStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FAI_Toolkit_InterfaceStyle::StyleInstance = nullptr;

void FAI_Toolkit_InterfaceStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FAI_Toolkit_InterfaceStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FAI_Toolkit_InterfaceStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("AI_Toolkit_InterfaceStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FAI_Toolkit_InterfaceStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("AI_Toolkit_InterfaceStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("AI_Toolkit_Interface")->GetBaseDir() / TEXT("Resources"));

	Style->Set("AI_Toolkit_Interface.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	return Style;
}

void FAI_Toolkit_InterfaceStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FAI_Toolkit_InterfaceStyle::Get()
{
	return *StyleInstance;
}
