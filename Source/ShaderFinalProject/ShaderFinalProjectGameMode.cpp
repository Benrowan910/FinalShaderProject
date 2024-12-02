// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShaderFinalProjectGameMode.h"
#include "ShaderFinalProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AShaderFinalProjectGameMode::AShaderFinalProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
