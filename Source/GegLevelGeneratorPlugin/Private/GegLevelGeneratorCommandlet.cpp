// Fill out your copyright notice in the Description page of Project Settings.


#include "GegLevelGeneratorCommandlet.h"
#include "AssetRegistryModule.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "GegLevelGeneratorPlugin.h"

int32 UGegLevelGeneratorCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Error, TEXT("Custom Commandlet parameters: %s"), *Params);

	TArray<FString> Out;
	Params.ParseIntoArray(Out, TEXT(" "), true);
	UE_LOG(LogTemp, Warning, TEXT("Path file map: %s"), *Out[1]);

	TArray<FString> FileRows;
	if (! FFileHelper::LoadFileToStringArray(FileRows, *Out[1]))
	{
		UE_LOG(LogTemp, Warning, TEXT("Error in loading file map"));
		return 1;
	}

	UWorld* NewMap = Level.GenerateNewWorldFromTxt(&FileRows);
	
	if (NewMap)
	{
		UPackage* Package = NewMap->GetPackage();
		FString FileName = FPackageName::LongPackageNameToFilename(Package->GetPathName(), FPackageName::GetAssetPackageExtension());

		bool bSaved = UPackage::SavePackage(Package, NewMap, EObjectFlags::RF_Standalone | EObjectFlags::RF_Public, *FileName);
		if (bSaved)
		{
			FAssetRegistryModule::AssetCreated(NewMap);
			UE_LOG(LogTemp, Warning, TEXT("NewLevel saved in package: %s"), *Package->GetFName().ToString() );
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
