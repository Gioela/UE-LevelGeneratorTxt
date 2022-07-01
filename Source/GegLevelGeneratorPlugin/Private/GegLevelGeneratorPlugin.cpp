// Copyright Epic Games, Inc. All Rights Reserved.

#include "GegLevelGeneratorPlugin.h"
#include "AssetRegistryModule.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/WorldFactory.h"
#include "Materials/MaterialInstance.h"
#include "Misc/FileHelper.h"
#include "WorkspaceMenuStructureModule.h"
#include "WorkspaceMenuStructure.h"
#include "DesktopPlatformModule.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "FGegLevelGeneratorPluginModule"

static const FName LevelGenerator("Level Generator");

void FGegLevelGeneratorPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	AssetThumbnailPool = MakeShareable(new FAssetThumbnailPool(16, false));

	FTabSpawnerEntry TabSpawnerEntry = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LevelGenerator, FOnSpawnTab::CreateRaw(this, &FGegLevelGeneratorPluginModule::CreateMapGeneretorDockTab))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory());

	ValidateInputStaticMesh();
	PrjPath = TEXT("/GegLevelGenerator/Core");
}

void FGegLevelGeneratorPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	BreakableWallAsset = nullptr;
	UnbreakableWallAsset = nullptr;
	FloorAsset = nullptr;
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LevelGenerator);
}

bool FGegLevelGeneratorPluginModule::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FParse::Command(&Cmd, TEXT("createlevel")))
	{
		FString Path = FParse::Token(Cmd, true);
		CreateLevelFromTxt(*Path);
		return true;
	}
	return false;
}

TSharedRef<SDockTab> FGegLevelGeneratorPluginModule::CreateMapGeneretorDockTab(const FSpawnTabArgs& TabArgs)
{
	TSharedPtr<SDockTab> Window = SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SBorder).Padding(15)
			[
				// BREAKABLE WALL MATERIAL
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 8).HAlign(EHorizontalAlignment::HAlign_Left)
				[
					SNew(SBorder).Padding(15)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 8).HAlign(EHorizontalAlignment::HAlign_Left)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().Padding(5).VAlign(EVerticalAlignment::VAlign_Center)
							[
								SNew(STextBlock).Text(LOCTEXT("BreakableWallMaterial", "Breakable Wall Material"))
							]
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(SObjectPropertyEntryBox)
								.AllowedClass(UMaterialInstance::StaticClass())
								.AllowedClass(UMaterial::StaticClass())
								.DisplayThumbnail(true).ThumbnailPool(AssetThumbnailPool)
								.ObjectPath_Lambda([this]() -> FString
									{
										return BreakableWallMaterial.GetAsset()->GetPathName();
									})
								.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
									{
										BreakableWallMaterial = InNewAssetData;
									})
							]
						]
						// BREAKABLE WALL STATIC MESH
						+ SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 8).HAlign(EHorizontalAlignment::HAlign_Left)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().Padding(5).VAlign(EVerticalAlignment::VAlign_Center)
							[
								SNew(STextBlock).Text(LOCTEXT("BreakableWallBluePrint", "Breakable Wall BluePrint"))
							]
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(SObjectPropertyEntryBox)
								.AllowedClass(UBlueprint::StaticClass())
								.DisplayThumbnail(true).ThumbnailPool(AssetThumbnailPool)
								.ObjectPath_Lambda([this]() -> FString
									{
										return BreakableWallAsset.GetAsset()->GetPathName();
									})
								.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
									{
										BreakableWallAsset = InNewAssetData;
									})
							]
						]
					]
				]

				// UNBREAKABLE WALL DETAILS
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 8).HAlign(EHorizontalAlignment::HAlign_Left)
				[
					SNew(SBorder).Padding(15)
					[
						SNew(SVerticalBox)
						// UNBREAKABLE WALL MATERIAL
						+ SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 8).HAlign(EHorizontalAlignment::HAlign_Left)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().Padding(5).VAlign(EVerticalAlignment::VAlign_Center)
							[
								SNew(STextBlock).Text(LOCTEXT("UnbreakableWallMaterial", "Unbreakable Wall Material"))
							]
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(SObjectPropertyEntryBox)
								.AllowedClass(UMaterial::StaticClass())
								.AllowedClass(UMaterialInstance::StaticClass())
								.DisplayThumbnail(true).ThumbnailPool(AssetThumbnailPool)
								.ObjectPath_Lambda([this]() -> FString
									{
										return UnbreakableWallMaterial.GetAsset()->GetPathName();
									})
								.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
									{
										UnbreakableWallMaterial = InNewAssetData;
									})
							]
						]
						// UNBREAKABLE WALL STATIC MESH
						+ SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 8).HAlign(EHorizontalAlignment::HAlign_Left)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().Padding(5).VAlign(EVerticalAlignment::VAlign_Center)
							[
								SNew(STextBlock).Text(LOCTEXT("UnbreakableWallStaticMesh", "Unbreakable Wall StaticMesh"))
							]
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(SObjectPropertyEntryBox)
								.AllowedClass(UStaticMesh::StaticClass())
								.DisplayThumbnail(true).ThumbnailPool(AssetThumbnailPool)
								.ObjectPath_Lambda([this]() -> FString
									{
										return UnbreakableWallAsset.GetAsset()->GetPathName();
									})
								.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
									{
										UnbreakableWallAsset = InNewAssetData;
									})
							]
						]
					]
				]

				// FLOOR MATERIAL
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 8).HAlign(EHorizontalAlignment::HAlign_Left)
				[
					SNew(SBorder).Padding(15)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 8).HAlign(EHorizontalAlignment::HAlign_Left)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().Padding(5).VAlign(EVerticalAlignment::VAlign_Center)
							[
								SNew(STextBlock).Text(LOCTEXT("FloorMaterial", "Floor Material"))
							]
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(SObjectPropertyEntryBox)
							.AllowedClass(UMaterial::StaticClass())
							.AllowedClass(UMaterialInstance::StaticClass())
							.DisplayThumbnail(true).ThumbnailPool(AssetThumbnailPool)
							.ObjectPath_Lambda([this]() -> FString
								{
									return FloorMaterial.GetAsset()->GetPathName();
								})
							.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
								{
									FloorMaterial = InNewAssetData;
								})
						]
					]
					// FLOOR STATIC MESH
					+ SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 8).HAlign(EHorizontalAlignment::HAlign_Left)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().Padding(5).VAlign(EVerticalAlignment::VAlign_Center)
						[
							SNew(STextBlock).Text(LOCTEXT("FloorStaticMesh", "Floor Static Mesh"))
						]
					+ SHorizontalBox::Slot().AutoWidth()
					[
						SNew(SObjectPropertyEntryBox)
						.AllowedClass(UStaticMesh::StaticClass())
						.DisplayThumbnail(true).ThumbnailPool(AssetThumbnailPool)
						.ObjectPath_Lambda([this]() -> FString
							{
								return FloorAsset.GetAsset()->GetPathName();
							})
						.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
							{
								FloorAsset = InNewAssetData;
							})
					]
				]
			]
		]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Load Tiled File")))
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.VAlign(EVerticalAlignment::VAlign_Center)
					.OnClicked(FOnClicked::CreateRaw(this, &FGegLevelGeneratorPluginModule::OnClickLoadMapFile))
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Generate Level")))
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.VAlign(EVerticalAlignment::VAlign_Center)
					.OnClicked(FOnClicked::CreateRaw(this, &FGegLevelGeneratorPluginModule::OnClickGenerateMapLevel))
				]
			]
		]
	];
	return Window.ToSharedRef();
}

FReply FGegLevelGeneratorPluginModule::OnClickLoadMapFile()
{
	OpenFileManagerMenu(TEXT("Choiche the txt file with map description"), TEXT(""), TEXT("Map Files|*txt"), FileLevelEditor);
	UE_LOG(LogTemp, Warning, TEXT("FileMap loaded correctly"));

	FText TitleMsg = FText::FromString("Info");
	FText Msg = FText::FromString("FileMap loaded");
	FMessageDialog::Open(EAppMsgType::Ok, Msg, &TitleMsg);

	return FReply::Handled();
}

FReply FGegLevelGeneratorPluginModule::OnClickGenerateMapLevel()
{
	if (FileLevelEditor.Num() != 0)
	{
		CreateLevelFromTxt(FileLevelEditor[0]);

		FText TitleMsg = FText::FromString("Info");
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("New Level was created"), &TitleMsg);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MapFile is not loaded yet"));
		FText TitleMsg = FText::FromString("Alert Generate New Level");
		FText Msg = FText::FromString("Please, load map level file before generate a new level.");
		FMessageDialog::Open(EAppMsgType::Ok, Msg, &TitleMsg);
	}
	return FReply::Handled();
}

void FGegLevelGeneratorPluginModule::OpenFileManagerMenu(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	OutFileNames.Empty();
	DesktopPlatform->OpenFileDialog(nullptr, DialogTitle, DefaultPath, FString(""), FileTypes, 0, OutFileNames);
	//FGlobalTabmanager::Get()->TryInvokeTab(MapGenerator);
}

void FGegLevelGeneratorPluginModule::CreateLevelFromTxt(FString InPath)
{
	TArray<FString> FileRows;
	FFileHelper::LoadFileToStringArray(FileRows, *InPath);

	UE_LOG(LogTemp, Error, TEXT("MapFile Path: %s"), *InPath);

	if (FileRows.Num() > 0)
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

		for (FString Line : FileRows)
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
		WorldCasted->PostEditChange();
		WorldCasted->MarkPackageDirty();
		UE_LOG(LogTemp, Warning, TEXT("New Level created correctly: %s"), *WorldCasted->GetName());
	}
}

void FGegLevelGeneratorPluginModule::SetLevelDefaultLights(UWorld* InWorld)
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

void FGegLevelGeneratorPluginModule::ValidateInputStaticMesh()
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
AActor* FGegLevelGeneratorPluginModule::CreateGamePlatform(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum)
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

AActor* FGegLevelGeneratorPluginModule::CreateUnbreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum)
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

AActor* FGegLevelGeneratorPluginModule::CreateBreakableWall(UWorld* InWorld, const int32 InPosX, const int32 InPosY, const uint32 TileNum)
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
		ABreakableWall->GetStaticMeshComponent()->SetMaterial(0, Cast<UMaterial>(BreakableWallMaterial.GetAsset()));
		return ABreakableWall;
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGegLevelGeneratorPluginModule, GegLevelGeneratorPlugin)