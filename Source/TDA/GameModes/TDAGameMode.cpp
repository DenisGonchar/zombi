// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDAGameMode.h"
#include "../Controllers/TDAPlayerController.h"
#include "../Characters/TDACharacter.h"
#include "UObject/ConstructorHelpers.h"

ATDAGameMode::ATDAGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATDAPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/Character/BP_Character"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}