// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GegLevelGeneratorCommandlet.generated.h"

/**
 * 
 */
UCLASS()
class GEGLEVELGENERATORPLUGIN_API UGegLevelGeneratorCommandlet : public UCommandlet
{
	GENERATED_BODY()
	
public:
	int32 Main(const FString& Params) override;


private:
	UWorld* CreateLevelFromTxt(const TArray<FString>* FileRows);
	void SetLevelDefaultLights(UWorld* InWorld);
	void ValidateInputStaticMesh();

	AActor* CreateGamePlatform(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum);
	AActor* CreateUnbreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum);
	AActor* CreateBreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum);

	FAssetData BreakableWallMaterial;
	FAssetData BreakableWallAsset;

	FAssetData UnbreakableWallMaterial;
	FAssetData UnbreakableWallAsset;

	FAssetData FloorMaterial;
	FAssetData FloorAsset;
};
