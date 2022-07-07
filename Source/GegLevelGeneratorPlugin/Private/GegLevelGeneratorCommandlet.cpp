// Fill out your copyright notice in the Description page of Project Settings.


#include "GegLevelGeneratorCommandlet.h"
#include "AssetRegistryModule.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/WorldFactory.h"
#include "Misc/FileHelper.h"
#include "GegLevelGeneratorPlugin.h"

int32 UGegLevelGeneratorCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Error, TEXT("Custom Commandlet parameters: %s"), *Params);

	TArray<FString> Out;
	Params.ParseIntoArray(Out, TEXT(" "), true);
	UE_LOG(LogTemp, Warning, TEXT("Path file map: %s"), *Out[1]);

	//FString Left, Right;
	//Params.Split(TEXT(" "), &Left, &Right);
	//// Right.Split(TEXT(" "), &Left, &Right);
	//UE_LOG(LogTemp, Warning, TEXT("2a - Left String map: %s"), *Left);
	//UE_LOG(LogTemp, Warning, TEXT("2a - Right String map: %s"), *Right);

	TArray<FString> FileRows;
	if (! FFileHelper::LoadFileToStringArray(FileRows, *Out[1]))
	{
		UE_LOG(LogTemp, Warning, TEXT("Error in loading file map"));
		return 1;
	}

	// UWorld* NewMap = FGegLevelGeneratorPluginModule::Get().CreateWorldFromTxt(&FileRows);
	UWorld* NewMap = CreateLevelFromTxt(&FileRows);
	
	if (NewMap)
	{
		//NewMap->SaveConfig();
		UPackage* Package = NewMap->GetPackage();
		FString FileName = TEXT("Pippo");
		//FString FileName = FPackageName::LongPackageNameToFilename(Package->GetPathName(), FPackageName::GetAssetPackageExtension());

		bool bSaved = UPackage::SavePackage(Package, NewMap, EObjectFlags::RF_Standalone | EObjectFlags::RF_Public, *FileName);
		if (bSaved)
		{
			FAssetRegistryModule::AssetCreated(NewMap);
			UE_LOG(LogTemp, Warning, TEXT("NewLevel saved"));
			return 0;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error NewLevel UNSAVED"));
			return -1;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("NO NewLevel to save"));

	return 0;
}

UWorld* UGegLevelGeneratorCommandlet::CreateLevelFromTxt(const TArray<FString>* FileRows)
{
	UWorldFactory* WorldFactory = NewObject<UWorldFactory>();
	uint64 SuffixAssetName = FPlatformTime::Cycles64();
	FString AssetName = FString::Printf(TEXT("Level_%llu"), SuffixAssetName);
	// FString MapPackage = FString::Printf(TEXT("%s/Maps/"), PrjPath);
	//UPackage* Package = CreatePackage(*FString::Printf(TEXT("%s/Maps/%s/%s"), *PrjPath, *AssetName, *AssetName));
	UPackage* Package = CreatePackage(*FString::Printf(TEXT("/Game/GegLevelGenerator/Maps/%s/%s"), *AssetName, *AssetName));
	//PackageLevel = CreatePackage(*FString::Printf(TEXT("/Game/Core/Maps/%s/%s"), *AssetName, *AssetName));

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

	// Create Default static mesh from the input values
	ValidateInputStaticMesh();

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


void UGegLevelGeneratorCommandlet::SetLevelDefaultLights(UWorld* InWorld)
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

void UGegLevelGeneratorCommandlet::ValidateInputStaticMesh()
{
	// check if floor static mesh is valid
	if (!FloorAsset.IsValid())
	{
		FloorAsset = LoadObject<UStaticMesh>(nullptr, *FString("StaticMesh'/GegLevelGeneratorPlugin/GegCore/Meshes/SM_Floor_1x1.SM_Floor_1x1'"));
	}

	if (!UnbreakableWallAsset.IsValid())
	{
		UnbreakableWallAsset = LoadObject<UStaticMesh>(nullptr, *FString("StaticMesh'/GegLevelGeneratorPlugin/GegCore/Meshes/SM_Cube_1x1.SM_Cube_1x1'"));
	}

	if (!BreakableWallAsset.IsValid())
	{
		BreakableWallAsset = LoadObject<UBlueprint>(nullptr, *FString("Blueprint'/GegLevelGeneratorPlugin/GegCore/Blueprints/BP_Wall.BP_Wall'"));
	}
}

AActor* UGegLevelGeneratorCommandlet::CreateGamePlatform(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum)
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

AActor* UGegLevelGeneratorCommandlet::CreateUnbreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum)
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

AActor* UGegLevelGeneratorCommandlet::CreateBreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum)
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