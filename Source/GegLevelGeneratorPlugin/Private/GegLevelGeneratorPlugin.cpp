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
#include "Materials/Material.h"
#include "Misc/FileHelper.h"
#include "WorkspaceMenuStructureModule.h"
#include "WorkspaceMenuStructure.h"
#include "DesktopPlatformModule.h"
#include "PropertyCustomizationHelpers.h"
#include "GegSlateInitialization.h"
#include "Textures/SlateIcon.h"

#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "FGegLevelGeneratorPluginModule"

static const FName LevelGenerator("Level Generator");

void FGegLevelGeneratorPluginModule::StartupModule()
{
	GegSlateInitialization::Initialize();
	GegSlateInitialization::ReloadTextures();

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LevelGenerator, FOnSpawnTab::CreateRaw(this, &FGegLevelGeneratorPluginModule::CreateMapGeneretorDockTab))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	TSharedRef<FExtender> ToolBarExtender = MakeShared<FExtender>();
	ToolBarExtender->AddToolBarExtension("Settings", EExtensionHook::After, nullptr,
		FToolBarExtensionDelegate::CreateLambda([this](FToolBarBuilder& Builder)
			{
				Builder.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this]()
					{
						FGlobalTabmanager::Get()->InvokeTab(LevelGenerator);
					})), NAME_None, FText::FromString("Level Generator"));
			}));

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolBarExtender);

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	AssetThumbnailPool = MakeShareable(new FAssetThumbnailPool(16, false));

	FTabSpawnerEntry TabSpawnerEntry = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LevelGenerator, FOnSpawnTab::CreateRaw(this, &FGegLevelGeneratorPluginModule::CreateMapGeneretorDockTab))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory());

	//PrjPath = TEXT("/GegLevelGenerator/Core");
}

void FGegLevelGeneratorPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	NewLevel.~GegCoreLevel();
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
								.AllowedClass(UMaterialInterface::StaticClass())
								.DisplayThumbnail(true).ThumbnailPool(AssetThumbnailPool)
								.ObjectPath_Lambda([this]() -> FString
									{
										// return BreakableWallMaterial.GetAsset()->GetPathName();
										return NewLevel.BreakableWallMaterial.GetAsset()->GetPathName();
									})
								.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
									{
										//BreakableWallMaterial = InNewAssetData;
										NewLevel.BreakableWallMaterial = InNewAssetData;
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
										//return BreakableWallAsset.GetAsset()->GetPathName();
										return NewLevel.BreakableWallAsset.GetAsset()->GetPathName();
									})
								.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
									{
										NewLevel.BreakableWallAsset = InNewAssetData;
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
								.AllowedClass(UMaterialInterface::StaticClass())
								.DisplayThumbnail(true).ThumbnailPool(AssetThumbnailPool)
								.ObjectPath_Lambda([this]() -> FString
									{
										return NewLevel.UnbreakableWallMaterial.GetAsset()->GetPathName();
									})
								.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
									{
										NewLevel.UnbreakableWallMaterial = InNewAssetData;
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
										return NewLevel.UnbreakableWallAsset.GetAsset()->GetPathName();
									})
								.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
									{
										NewLevel.UnbreakableWallAsset = InNewAssetData;
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
							.AllowedClass(UMaterialInterface::StaticClass())
							.DisplayThumbnail(true).ThumbnailPool(AssetThumbnailPool)
							.ObjectPath_Lambda([this]() -> FString
								{
									return NewLevel.FloorMaterial.GetAsset()->GetPathName();
								})
							.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
								{
									NewLevel.FloorMaterial = InNewAssetData;
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
								return NewLevel.FloorAsset.GetAsset()->GetPathName();
							})
						.OnObjectChanged_Lambda([this](const FAssetData& InNewAssetData) -> void
							{
								NewLevel.FloorAsset = InNewAssetData;
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
	if (!FFileHelper::LoadFileToStringArray(FileRows, *InPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Error in loading file map"));
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("MapFile Path: %s"), *InPath);

	if (FileRows.Num() > 0)
	{
		UWorld* NewWorld = NewLevel.GenerateNewWorldFromTxt(&FileRows);
		NewWorld->PostEditChange();
		NewWorld->MarkPackageDirty();
		UE_LOG(LogTemp, Warning, TEXT("New Level created correctly: %s"), *NewWorld->GetName());
	}
}

FGegLevelGeneratorPluginModule& FGegLevelGeneratorPluginModule::Get()
{
	static FGegLevelGeneratorPluginModule* Singleton = nullptr;
	if (!Singleton)
	{
		Singleton = &FModuleManager::LoadModuleChecked<FGegLevelGeneratorPluginModule>("GegLevelGeneratorPluginModule");
	}
	return *Singleton;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGegLevelGeneratorPluginModule, GegLevelGeneratorPlugin)