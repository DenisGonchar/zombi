// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDAPlayerController.generated.h"

UCLASS()
class ATDAPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATDAPlayerController();

	virtual void SetPawn(APawn* InPawn) override;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	
	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();
	
	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	
	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

protected:
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	void TryStartFire();
	void TryEndFire();
	//Sprint
	void StartSprint();
	void EndSprint();

	//Walk
	void StartWalk();
	void EndWalk();
	
	//Reload 
	void ReloadWeapon();

	void SwitchNextWeapon();
	void SwitchPreviosWeapon();
	
	TSoftObjectPtr<class ATDACharacter> CachedBaseCharacter;

};


