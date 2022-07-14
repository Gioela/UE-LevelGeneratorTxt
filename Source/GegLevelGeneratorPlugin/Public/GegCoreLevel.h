// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetThumbnail.h"

/**
 * 
 */
class GEGLEVELGENERATORPLUGIN_API GegCoreLevel
{
public:
	GegCoreLevel();
	~GegCoreLevel();

	UWorld* GenerateNewWorldFromTxt(const TArray<FString>* FileRows);
	//void Delete();
	
protected:
	void InitBreakableWallProperties();
	void InitUnbreakableWallProperties();
	void InitFloorProperties();

	void ValidateAssets();
	void SetLevelDefaultLights(UWorld* InWorld);

	AActor* CreateGamePlatform(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum);
	AActor* CreateUnbreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum);
	AActor* CreateBreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum);

public:
	FAssetData BreakableWallMaterial;
	FAssetData BreakableWallAsset;

	FAssetData UnbreakableWallMaterial;
	FAssetData UnbreakableWallAsset;

	FAssetData FloorMaterial;
	FAssetData FloorAsset;
};
