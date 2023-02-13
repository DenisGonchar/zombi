// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDAPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "../Characters/TDACharacter.h"
#include "Engine/World.h"

ATDAPlayerController::ATDAPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ATDAPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	CachedBaseCharacter = Cast<ATDACharacter>(InPawn);

}

void ATDAPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void ATDAPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ATDAPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATDAPlayerController::MoveRight);

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ATDAPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ATDAPlayerController::OnSetDestinationReleased);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ATDAPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ATDAPlayerController::EndSprint);
	InputComponent->BindAction("Walk", IE_Pressed, this, &ATDAPlayerController::StartWalk);
	InputComponent->BindAction("Walk", IE_Released, this, &ATDAPlayerController::EndWalk);
	InputComponent->BindAction("Fire", IE_Pressed, this, &ATDAPlayerController::TryStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &ATDAPlayerController::TryEndFire);
	InputComponent->BindAction("Reload",IE_Pressed, this, &ATDAPlayerController::ReloadWeapon);
	InputComponent->BindAction("SwitchNextWeapon", IE_Pressed, this, &ATDAPlayerController::SwitchNextWeapon);
	InputComponent->BindAction("SwitchPreviosWeapon", IE_Pressed, this, &ATDAPlayerController::SwitchPreviosWeapon);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATDAPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ATDAPlayerController::MoveToTouchLocation);
	
}

void ATDAPlayerController::MoveToMouseCursor()
{
	//if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	//{
	//	if (ATDACharacter* MyPawn = Cast<ATDACharacter>(GetPawn()))
	//	{
	//		if (MyPawn->GetCursorToWorld())
	//		{
	//			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
	//		}
	//	}
	//}
	//else
	//{
	//	// Trace to see what is under the mouse cursor
	//	FHitResult Hit;
	//	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	//	if (Hit.bBlockingHit)
	//	{
	//		// We hit something, move there
	//		SetNewMoveDestination(Hit.ImpactPoint);
	//	}
	//}
}

void ATDAPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	//FVector2D ScreenSpaceLocation(Location);

	//// Trace to see what is under the touch location
	//FHitResult HitResult;
	//GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	//if (HitResult.bBlockingHit)
	//{
	//	// We hit something, move there
	//	SetNewMoveDestination(HitResult.ImpactPoint);
	//}
}

void ATDAPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ATDAPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ATDAPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void ATDAPlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
	}

}

void ATDAPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void ATDAPlayerController::TryStartFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InputAttackPressed();
	}
}

void ATDAPlayerController::TryEndFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InputAttackReleased();
	}
}

void ATDAPlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void ATDAPlayerController::EndSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EndSprint();
	}
}

void ATDAPlayerController::StartWalk()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartWalk();
	}
}

void ATDAPlayerController::EndWalk()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EndWalk();
	}
}

void ATDAPlayerController::ReloadWeapon()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TryReloadWeapon();
	}

}

void ATDAPlayerController::SwitchNextWeapon()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TrySwicthNextWeapon();
	}
}

void ATDAPlayerController::SwitchPreviosWeapon()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TrySwicthPreviosWeapon();
	}
}
