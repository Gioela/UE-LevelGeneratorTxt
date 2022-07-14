// Fill out your copyright notice in the Description page of Project Settings.


#include "GegCoreLevel.h"
#include "AssetRegistryModule.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/WorldFactory.h"
#include "Misc/FileHelper.h"

FString DefaultFloorAssetPath = TEXT("StaticMesh'/GegLevelGeneratorPlugin/GegCore/Meshes/SM_Floor_1x1.SM_Floor_1x1'");
FString DefaultUnbreakableWallAssetPath = TEXT("StaticMesh'/GegLevelGeneratorPlugin/GegCore/Meshes/SM_Cube_1x1.SM_Cube_1x1'");
FString DefaultBreakableWallAssetPath = TEXT("Blueprint'/GegLevelGeneratorPlugin/GegCore/Blueprints/BP_Wall.BP_Wall'");

GegCoreLevel::GegCoreLevel()
{
	InitFloorProperties();
	InitUnbreakableWallProperties();
	InitBreakableWallProperties();
}

GegCoreLevel::~GegCoreLevel()
{
	FloorMaterial = nullptr;
	FloorAsset = nullptr;

	UnbreakableWallMaterial = nullptr;
	UnbreakableWallAsset = nullptr;

	BreakableWallMaterial = nullptr;
	BreakableWallAsset = nullptr;
}

//void GegCoreLevel::Delete()
//{
//	~GegCoreLevel();
//}

void GegCoreLevel::InitFloorProperties()
{
	FloorMaterial = nullptr;
	FloorAsset = LoadObject<UStaticMesh>(nullptr, *DefaultFloorAssetPath);
}

void GegCoreLevel::InitUnbreakableWallProperties()
{
	UnbreakableWallMaterial = nullptr;
	UnbreakableWallAsset = LoadObject<UStaticMesh>(nullptr, *DefaultUnbreakableWallAssetPath);
}

void GegCoreLevel::InitBreakableWallProperties()
{
	BreakableWallMaterial = nullptr;
	BreakableWallAsset = LoadObject<UBlueprint>(nullptr, *DefaultBreakableWallAssetPath);
}

/*
* If class assets are not valid, load the default
*/
void GegCoreLevel::ValidateAssets()
{
	if (!FloorAsset.IsValid())
	{
		FloorAsset = LoadObject<UStaticMesh>(nullptr, *DefaultFloorAssetPath);
	}

	if (!UnbreakableWallAsset.IsValid())
	{
		UnbreakableWallAsset = LoadObject<UStaticMesh>(nullptr, *DefaultUnbreakableWallAssetPath);
	}

	if (!BreakableWallAsset.IsValid())
	{
		BreakableWallAsset = LoadObject<UBlueprint>(nullptr, *DefaultBreakableWallAssetPath);
	}
}

UWorld* GegCoreLevel::GenerateNewWorldFromTxt(const TArray<FString>* FileRows)
{
	UWorldFactory* WorldFactory = NewObject<UWorldFactory>();
	uint64 SuffixAssetName = FPlatformTime::Cycles64();
	FString AssetName = FString::Printf(TEXT("Level_%llu"), SuffixAssetName);
	UPackage* Package = CreatePackage(*FString::Printf(TEXT("/Game/GegLevelGenerator/Maps/%s/%s"), *AssetName, *AssetName));

	UObject* NewLevelObject = WorldFactory->FactoryCreateNew(WorldFactory->SupportedClass, Package, *AssetName, EObjectFlags::RF_Standalone | EObjectFlags::RF_Public, nullptr, GWarn);
	FAssetRegistryModule::AssetCreated(NewLevelObject);
	UWorld* WorldCasted = Cast<UWorld>(NewLevelObject);
	WorldCasted->Modify();

	SetLevelDefaultLights(WorldCasted);

	int32 StartX = 0;
	int32 StartY = 0;
	int32 Size = 100;
	AActor* Floor = nullptr;
	AActor* WallUnbreakable = nullptr;
	AActor* WallBreakable = nullptr;

	uint32 TileNumber = 0;

	ValidateAssets();

	for (FString Line : *FileRows)
	{
		TArray<FString> BlockInLine;
		Line.ParseIntoArray(BlockInLine, TEXT(","));
		for (int32 Index = 0; Index < BlockInLine.Num(); Index++)
		{
			Floor = CreateGamePlatform(WorldCasted, StartX * Size, StartY * Size, TileNumber);
			FAssetRegistryModule::AssetCreated(Floor);

			if (BlockInLine[Index].Equals(TEXT("3")))
			{
				WallUnbreakable = CreateUnbreakableWall(WorldCasted, StartX * Size, StartY * Size, TileNumber);
				FAssetRegistryModule::AssetCreated(WallUnbreakable);
			}
			else if (BlockInLine[Index].Equals(TEXT("2")))
			{
				WallBreakable = CreateBreakableWall(WorldCasted, StartX * Size, StartY * Size, TileNumber);
				FAssetRegistryModule::AssetCreated(WallBreakable);
			}
			++StartX;
			++TileNumber;
		}
		++StartY;
		StartX = 0;
	}

	return WorldCasted;
}

/*
* Set level default lights
*/
void GegCoreLevel::SetLevelDefaultLights(UWorld* InWorld)
{
	FName LevelLightPath = TEXT("/Lighting");

	ADirectionalLight* DirectionalLight = InWorld->SpawnActor<ADirectionalLight>();
	DirectionalLight->SetFolderPath(LevelLightPath);
	UDirectionalLightComponent* DirLightComponent = DirectionalLight->GetComponent();
	DirLightComponent->SetAtmosphereSunLight(true);

	ASkyLight* SkyLight = InWorld->SpawnActor<ASkyLight>();
	SkyLight->SetFolderPath(LevelLightPath);

	/* AtmosphericFog will be DEPRECATED
	AAtmosphericFog* AtmosphericFog = WorldCasted->SpawnActor<AAtmosphericFog>();
	AtmosphericFog->SetFolderPath(LevelLightPath);
	*/
	ASkyAtmosphere* SkyAtmosphere = InWorld->SpawnActor<ASkyAtmosphere>();
	SkyAtmosphere->SetFolderPath(LevelLightPath);

	AExponentialHeightFog* ExponentialHeightFog = InWorld->SpawnActor<AExponentialHeightFog>();
	ExponentialHeightFog->SetFolderPath(LevelLightPath);
}


AActor* GegCoreLevel::CreateGamePlatform(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum)
{
	FName LevelActorPath = TEXT("/Floor");
	FVector ActorLocation = FVector(InPosX, InPosY, -50);

	FActorSpawnParameters FloorActorParameters;
	FloorActorParameters.Name = *(TEXT("Floor_") + FString::Printf(TEXT("_%d"), TileNum));

	AStaticMeshActor* AFloor = InWorld->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FloorActorParameters);
	AFloor->SetFolderPath(LevelActorPath);

	//AFloor->StaticMeshComponentName = FName(TEXT("Floor"));
	AFloor->SetActorLocation(ActorLocation);
	AFloor->GetStaticMeshComponent()->SetStaticMesh(Cast<UStaticMesh>(FloorAsset.GetAsset()));
	AFloor->GetStaticMeshComponent()->SetMaterial(0, Cast<UMaterialInterface>(FloorMaterial.GetAsset()));

	return AFloor;
}

AActor* GegCoreLevel::CreateUnbreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum)
{
	FName LevelActorPath = TEXT("/UnbreakableWall");
	FVector ActorLocation = FVector(InPosX, InPosY, 0);

	FActorSpawnParameters UnbreakableWallActorParameters;
	UnbreakableWallActorParameters.Name = *(TEXT("UnbreakableWall_") + FString::Printf(TEXT("_%d"), TileNum));

	AStaticMeshActor* AUnbreakableWall = InWorld->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), UnbreakableWallActorParameters);
	AUnbreakableWall->SetFolderPath(LevelActorPath);

	AUnbreakableWall->SetActorLocation(ActorLocation);
	AUnbreakableWall->GetStaticMeshComponent()->SetStaticMesh(Cast<UStaticMesh>(UnbreakableWallAsset.GetAsset()));
	AUnbreakableWall->GetStaticMeshComponent()->SetMaterial(0, Cast<UMaterialInterface>(UnbreakableWallMaterial.GetAsset()));

	return AUnbreakableWall;
}

AActor* GegCoreLevel::CreateBreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum)
{
	FName LevelActorPath = TEXT("/BreakableWall");
	FVector ActorLocation = FVector(InPosX, InPosY, 0);

	UBlueprint* BPBreakableWall = LoadObject<UBlueprint>(nullptr, *BreakableWallAsset.ObjectPath.ToString());
	if (BPBreakableWall && BPBreakableWall->GeneratedClass->IsChildOf<AActor>())
	{
		FActorSpawnParameters BreakableWallActorParameters;
		BreakableWallActorParameters.Name = *(TEXT("BreakableWall") + FString::Printf(TEXT("_%d"), TileNum));

		AStaticMeshActor* ABreakableWall = InWorld->SpawnActor<AStaticMeshActor>(BPBreakableWall->GeneratedClass, BreakableWallActorParameters);
		ABreakableWall->SetFolderPath(LevelActorPath);
		ABreakableWall->SetActorLocation(ActorLocation);
		ABreakableWall->GetStaticMeshComponent()->SetMaterial(0, Cast<UMaterialInterface>(BreakableWallMaterial.GetAsset()));
		return ABreakableWall;
	}
	return nullptr;
}