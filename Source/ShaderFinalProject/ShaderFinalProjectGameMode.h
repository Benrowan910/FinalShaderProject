// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShaderFinalProjectGameMode.generated.h"

UCLASS(minimalapi)
class AShaderFinalProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShaderFinalProjectGameMode();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isStormy;
};



