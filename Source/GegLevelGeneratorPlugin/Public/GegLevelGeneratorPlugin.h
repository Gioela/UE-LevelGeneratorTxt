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

	AActor* CreateGamePlatform(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum);
	AActor* CreateUnbreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum);
	AActor* CreateBreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum);

	void OpenFileManagerMenu(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames);
	void CreateLevelFromTxt(FString InPath);
	void SetLevelDefaultLights(UWorld* InWorld);
	void ValidateInputStaticMesh();

	UWorld* CreateWorldFromTxt(TArray<FString>* FileRows);
	//static UWorld* CreateWorldFromTxtS(TArray<FString>* FileRows);

	static FGegLevelGeneratorPluginModule& Get();

public:
	GegCoreLevel NewLevel;

	FString PrjPath;
	TSharedPtr<FAssetThumbnailPool> AssetThumbnailPool;

	FAssetData BreakableWallMaterial;
	FAssetData BreakableWallAsset;

	FAssetData UnbreakableWallMaterial;
	FAssetData UnbreakableWallAsset;

	FAssetData FloorMaterial;
	FAssetData FloorAsset;

	TArray<FString> FileLevelEditor;
};
