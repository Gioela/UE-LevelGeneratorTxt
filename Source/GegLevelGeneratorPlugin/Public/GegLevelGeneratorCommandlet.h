// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GegCoreLevel.h"
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
	GegCoreLevel Level;
};
