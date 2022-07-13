// Fill out your copyright notice in the Description page of Project Settings.


#include "GegSlateInitialization.h"
#include "GegLevelGeneratorPlugin.h"
//#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
#include "Slate/SlateGameResources.h"
#include "Styling/SlateStyleRegistry.h"

//GegSlateInitialization::GegSlateInitialization()
//{
//}
//
//GegSlateInitialization::~GegSlateInitialization()
//{
//}

TSharedPtr<FSlateStyleSet> GegSlateInitialization::StyleInstance = nullptr;

void GegSlateInitialization::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}

}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef<class FSlateStyleSet> GegSlateInitialization::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(TEXT("GegSlateInitialization")));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("GegLevelGeneratorPlugin")->GetBaseDir() / TEXT("Resources"));
	Style->Set("GegLevelGeneratorPlugin.PluginAction", new IMAGE_BRUSH(TEXT("LevGenIcon128"), Icon40x40));

	//UE_LOG(LogTemp, Error, TEXT("IconPath: %s"), IPluginManager::Get().FindPlugin("GegLevelGeneratorPlugin")->GetBaseDir() / TEXT("Resources"));

	return Style;
}

#undef IMAGE_BRUSH

const ISlateStyle& GegSlateInitialization::Get()
{
	return *StyleInstance;
}

void GegSlateInitialization::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}