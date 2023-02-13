// Fill out your copyright notice in the Description page of Project Settings.


#include "TDACharacterMovementComponent.h"
#include "../Characters/TDACharacter.h"

void UTDACharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	BaseCharacterOwner = Cast<ATDACharacter>(GetOwner());
}

float UTDACharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();
	if (bIsSprinting)
	{
		Result = SprintSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Green, FString::Printf(TEXT("Speed = %.2f"), Result));

	}
	if (bIsWalking)
	{
		Result = WalkSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Green, FString::Printf(TEXT("Speed = %.2f"), Result));

	}
	
	GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Green, FString::Printf(TEXT("Speed = %.2f"), Result));
	return Result;
}

void UTDACharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
}

void UTDACharacterMovementComponent::EndSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = 0;
}

void UTDACharacterMovementComponent::StartWalk()
{
	bIsWalking = true;
}

void UTDACharacterMovementComponent::EndWalk()
{
	bIsWalking = false;
	
}
