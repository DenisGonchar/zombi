// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Characters/TDACharacter.h"
#include "TDACharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class TDA_API UTDACharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	
	virtual void BeginPlay() override;

	virtual float GetMaxSpeed() const override;

	//Sprint
	FORCEINLINE bool IsSprinting() {return bIsSprinting;}
	
	void StartSprint();
	void EndSprint();

	//Walk
	FORCEINLINE bool IsWalking() { return bIsWalking; }

	void StartWalk();
	void EndWalk();


protected:
	UPROPERTY(Transient, DuplicateTransient)
	ATDACharacter* BaseCharacterOwner;

	//Sprint speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Sprint speed", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1000.0f;
	
	//Walk speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Walk speed", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float WalkSpeed = 300.0f;

private:
	//Sprint
	bool bIsSprinting = false;

	//Walk
	bool bIsWalking = false;

};
