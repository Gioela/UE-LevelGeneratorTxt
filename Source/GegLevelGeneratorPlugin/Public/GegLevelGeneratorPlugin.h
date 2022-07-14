// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetThumbnail.h"
#include "GegCoreLevel.h"
#include "Modules/ModuleManager.h"

class FGegLevelGeneratorPluginModule : public IModuleInterface, public FSelfRegisteringExec
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	TSharedRef<SDockTab> CreateMapGeneretorDockTab(const FSpawnTabArgs& TabArgs);
	FReply OnClickLoadMapFile();
	FReply OnClickGenerateMapLevel();

	void OpenFileManagerMenu(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames);
	void CreateLevelFromTxt(FString InPath);

	static FGegLevelGeneratorPluginModule& Get();

public:
	GegCoreLevel NewLevel;
	TSharedPtr<FAssetThumbnailPool> AssetThumbnailPool;
	TArray<FString> FileLevelEditor;
	
	//FString PrjPath;
};
