// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Types/Types.h"
#include "../Weapons/WeaponDefault.h"
#include "../InventoryComponent/TDAInventoryComponent.h"
#include "../HealthComponets/TDACharacterHealthComponent.h"
#include "TDACharacter.generated.h"

class UTDACharacterMovementComponent;

UCLASS(Blueprintable)
class ATDACharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATDACharacter(const FObjectInitializer& ObjectInitializer);
	
	FTimerHandle TimerHandle_RagDollTimer;
	
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	FORCEINLINE class UTDACharacterMovementComponent* GetBaseCharacterMovementComponent() { return BaseCharacterMovementComponent; }

	UFUNCTION(BlueprintCallable)
	UDecalComponent* GetCursorToWorld();
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UTDAInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UTDACharacterHealthComponent* CharHealthComponent;

private:
	

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UDecalComponent* CurrentCursor = nullptr;


	UTDACharacterMovementComponent* BaseCharacterMovementComponent;

public:
	
	//Cursor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);
	
	//Move
	virtual void MoveForward(float Value) { MoveForwardX = Value;}
	virtual void MoveRight(float Value) { MoveRightY = Value;};

	float MoveForwardX = 0.0f;
	float MoveRightY = 0.0f;

	
	void InputAttackPressed();
	void InputAttackReleased();

	//Sprint
	virtual void StartSprint();
	virtual void EndSprint();

	virtual bool CanSprint();

	//Walk
	virtual void StartWalk();
	virtual void EndWalk();

	virtual bool CanWalk();

	//Tick Func
	UFUNCTION()
	void MovementTick(float DeltaTime);
	void TryChangeSprintState(float DeltaTime);
	void TryChangeWalkState(float DeltaTime);

	void AttackCharEvent(bool bIsFiring);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TArray<UAnimMontage*>DeadsAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsAlive = true;

	//Weapon
	AWeaponDefault* CurrentWeapon = nullptr;

	UPROPERTY(editAnywhere, BlueprintReadWrite, category = "Demo")
	FName InitWeaponName;

	UFUNCTION(BlueprintCallable)
	AWeaponDefault* GetCurrentWeapon();
	UFUNCTION(BlueprintCallable)
	void InitWeapon(FName IdWeaponName , FAddicionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon );

	UFUNCTION(BlueprintCallable)
	void TryReloadWeapon();
	
	UFUNCTION()
		void WeaponFireStart(UAnimMontage* Anim);
	UFUNCTION()
		void WeaponReloadStart(UAnimMontage* Anim);
	UFUNCTION()
		void WeaponReloadEnd(bool bIsSuccess, int32 AmmoSafe);

	UFUNCTION(BlueprintNativeEvent)
		void WeaponFireStart_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
		void WeaponReloadStart_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
		void WeaponReloadEnd_BP(bool bIsSuccess);

	//Interface
	void TrySwicthNextWeapon();
	void TrySwicthPreviosWeapon();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 CurrentIndexWeapon = 0;

	//Character dead
	UFUNCTION(BlueprintCallable)
		void CharDead();

	void EnableRandoll();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	//Sprint
	bool bIsSprintRequested = false;
	
	//Walk
	bool bIsWalkRequested = false;

	bool bIsReload = false;
};

