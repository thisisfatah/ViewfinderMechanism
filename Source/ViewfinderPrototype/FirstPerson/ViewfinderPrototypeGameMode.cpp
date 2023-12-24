// Copyright Epic Games, Inc. All Rights Reserved.

#include "ViewfinderPrototypeGameMode.h"
#include "../ViewfinderPrototypeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AViewfinderPrototypeGameMode::AViewfinderPrototypeGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Viewfinder/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
