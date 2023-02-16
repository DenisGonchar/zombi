// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDACharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "../MovementComponets/TDACharacterMovementComponent.h"
#include <Math/UnrealMathUtility.h>
#include "../Weapons/WeaponDefault.h"
#include "../GameInstances/TDAGameInstance.h"
#include "../InventoryComponent/TDAInventoryComponent.h"
#include "../HealthComponets/TDACharacterHealthComponent.h"

ATDACharacter::ATDACharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTDACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	BaseCharacterMovementComponent = StaticCast<UTDACharacterMovementComponent*>(GetCharacterMovement());

	
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InventoryComponent = CreateDefaultSubobject<UTDAInventoryComponent>(TEXT("InventoryComponent"));

	CharHealthComponent = CreateDefaultSubobject<UTDACharacterHealthComponent>(TEXT("HealthComponent"));

	if (InventoryComponent)
	{
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ATDACharacter::InitWeapon);
	}
	if (CharHealthComponent)
	{
		CharHealthComponent->OnDead.AddDynamic(this, &ATDACharacter::CharDead);
	}
	// Activate ticking in order to update the cursor every frame.
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATDACharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CurrentCursor)
	{
		APlayerController* myPC = Cast<APlayerController>(GetController());
		if (myPC)
		{
			FHitResult TraceHitResult;
			myPC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();

			CurrentCursor->SetWorldLocation(TraceHitResult.Location);
			CurrentCursor->SetWorldRotation(CursorR);
		}
	}

	MovementTick(DeltaSeconds);
	TryChangeSprintState(DeltaSeconds);
	TryChangeWalkState(DeltaSeconds);
}

void ATDACharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
}

UDecalComponent* ATDACharacter::GetCursorToWorld()
{
	return CurrentCursor;
}

void ATDACharacter::InputAttackPressed()
{
	if (!GetBaseCharacterMovementComponent()->IsSprinting() && bIsAlive)
	{
		AttackCharEvent(true);
	}
	
}

void ATDACharacter::InputAttackReleased()
{
	AttackCharEvent(false);
}

void ATDACharacter::StartSprint()
{
	bIsSprintRequested = true;
	
}

void ATDACharacter::EndSprint()
{
	bIsSprintRequested = false;
}

bool ATDACharacter::CanSprint()
{	
	if (FMath::IsNearlyEqual(GetVelocity().Rotation().Yaw, GetActorRotation().Yaw, 30.0f))
	{	
		return true;

	}
	return false;
}

void ATDACharacter::StartWalk()
{
	bIsWalkRequested = true;
}

void ATDACharacter::EndWalk()
{
	bIsWalkRequested = false;
}

bool ATDACharacter::CanWalk()
{
	return !GetBaseCharacterMovementComponent()->IsSprinting();
}

void ATDACharacter::MovementTick(float DeltaTime)
{
	if (bIsAlive)
	{
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), MoveForwardX);
		AddMovementInput(FVector(0.0f, 1.0f, 0.0f), MoveRightY);

		APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (myController)
		{
			FHitResult ResultHit;
			myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);

			float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
			SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));

			if (CurrentWeapon)
			{
				FVector Displacement = FVector(0);
				if (!bIsWalkRequested && !bIsSprintRequested)
				{
					Displacement = FVector(0.0f, 0.0f, 120.0f);
				}
				if (bIsWalkRequested)
				{
					Displacement = FVector(0.0f, 0.0f, 120.0f);
				}
				CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
			}
		}
	}
}

void ATDACharacter::TryChangeSprintState(float DeltaTime)
{
	if (bIsSprintRequested && !BaseCharacterMovementComponent->IsSprinting() && CanSprint())
	{
		BaseCharacterMovementComponent->StartSprint();
		GetCurrentWeapon()->SetWeaponStateFire(false);
	
	}
	if (!(bIsSprintRequested && CanSprint()) && BaseCharacterMovementComponent->IsSprinting())
	{
		BaseCharacterMovementComponent->EndSprint();

	}
}

void ATDACharacter::TryChangeWalkState(float DeltaTime)
{
	if (bIsWalkRequested && !BaseCharacterMovementComponent->IsWalking() && CanWalk())
	{
		BaseCharacterMovementComponent->StartWalk();

	}
	if (!(bIsWalkRequested && CanWalk()) && BaseCharacterMovementComponent->IsWalking())
	{
		BaseCharacterMovementComponent->EndWalk();

	}

}

void ATDACharacter::AttackCharEvent(bool bIsFiring)
{
	
	AWeaponDefault* myWeapon = nullptr;
	myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->SetWeaponStateFire(bIsFiring);
	}


}

bool ATDACharacter::GetIsAlive()
{
	return bIsAlive;
}

AWeaponDefault* ATDACharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ATDACharacter::InitWeapon(FName IdWeaponName , FAddicionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	UTDAGameInstance* myGI = Cast<UTDAGameInstance>(GetGameInstance());
	FWeaponInfo myWeaponInfo;
	if (myGI)
	{
		if (myGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
		{

			if (myWeaponInfo.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myWeapon)
				{
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRight"));
					CurrentWeapon = myWeapon;
					myWeapon->ReloadTime = myWeaponInfo.ReloadTime;
					myWeapon->WeaponSetting = myWeaponInfo;

					myWeapon->WeaponInfo = WeaponAdditionalInfo;
					
					CurrentIndexWeapon = NewCurrentIndexWeapon;
					
					
					myWeapon->OnWeaponFireStart.AddDynamic(this, &ATDACharacter::WeaponFireStart);
					myWeapon->OnWeaponReloadStart.AddDynamic(this, &ATDACharacter::WeaponReloadStart);
					myWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATDACharacter::WeaponReloadEnd);

					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload() && bIsAlive)
					{
						bIsReload = true;
						CurrentWeapon->InitReload();
					}

					if (InventoryComponent)
					{
						InventoryComponent->OnWeaponAmmoAviable.Broadcast(myWeapon->WeaponSetting.WeaponType);
					}
				}

			}
		}
		else
		{

		}
	}
}

void ATDACharacter::TryReloadWeapon()
{
	if (CurrentWeapon && bIsAlive)
	{
		if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound && CurrentWeapon->CheckCanWeaponReload() && !bIsReload)
		{
			CurrentWeapon->InitReload();
			bIsReload = true;

		}
	}

}

void ATDACharacter::WeaponFireStart(UAnimMontage* Anim)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->WeaponInfo);
	}

	WeaponFireStart_BP(Anim);
}

void ATDACharacter::WeaponReloadStart(UAnimMontage* Anim)
{
	WeaponReloadStart_BP(Anim);
}

void ATDACharacter::WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake)
{
	if (InventoryComponent && CurrentWeapon)
	{
	InventoryComponent->AmmoSlotChangeValue(CurrentWeapon->WeaponSetting.WeaponType, AmmoTake);
	InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->WeaponInfo);
	}
	bIsReload = false;
	WeaponReloadEnd_BP(bIsSuccess);
}

void ATDACharacter::WeaponFireStart_BP_Implementation(UAnimMontage* Anim)
{

}

void ATDACharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{

}

void ATDACharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{

}

void ATDACharacter::TrySwicthNextWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		int8 OldIndex = CurrentIndexWeapon;
		FAddicionalWeaponInfo OldInfo;
		
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->WeaponInfo;
			if (CurrentWeapon->WeaponReloading)
			{
				CurrentWeapon->CancelReload();
			}
		}
		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo, true))
			{

			}
		}
	}
}

void ATDACharacter::TrySwicthPreviosWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		int8 OldIndex = CurrentIndexWeapon;
		FAddicionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->WeaponInfo;
			if (CurrentWeapon->WeaponReloading)
			{
				CurrentWeapon->CancelReload();
			}
		}
		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo, false))
			{

			}
		}
	}
}

void ATDACharacter::CharDead()
{
	float TimeAnim = 0.0f;

	int32 rnd = FMath::RandHelper(DeadsAnim.Num());
	if (DeadsAnim.IsValidIndex(rnd) && DeadsAnim[rnd] && GetMesh() && GetMesh()->GetAnimInstance())
	{
		TimeAnim = DeadsAnim[rnd]->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(DeadsAnim[rnd]);
	}

	bIsAlive = false;

	UnPossessed();

	GetWorldTimerManager().SetTimer(TimerHandle_RagDollTimer, this, &ATDACharacter::EnableRandoll, TimeAnim, false);

	GetCursorToWorld()->SetVisibility(false);
	InputAttackReleased();
}

void ATDACharacter::EnableRandoll()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}

float ATDACharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bIsAlive)
	{

		CharHealthComponent->ChangeHealthValue(-DamageAmount);

	}

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		AProjectileDefault* myProjectile = Cast<AProjectileDefault>(DamageCauser);
		if (myProjectile)
		{
			//UTypes::AddEffectBySurfaceType(this, myProjectile->ProjectileSetting.Effect, GetSurfaceType());

		}
	}


	return ActualDamage;
}

