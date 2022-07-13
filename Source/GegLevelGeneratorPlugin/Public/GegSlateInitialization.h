// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * 
 */
//class GEGLEVELGENERATORPLUGIN_API GegSlateInitialization
//{
//public:
//	GegSlateInitialization();
//	~GegSlateInitialization();
//};

class GegSlateInitialization
{
public:
	
	static void Initialize();
	static const ISlateStyle& Get();
	static void ReloadTextures();
	static TSharedPtr<class FSlateStyleSet> StyleInstance;

private:
	static TSharedRef<class FSlateStyleSet> Create();

};
