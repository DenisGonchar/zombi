// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDefault.h"
#include "../Projectiles/ProjectileDefault.h"
#include <Kismet/GameplayStatics.h>
#include <Engine/StaticMeshActor.h>
#include <Kismet/KismetMathLibrary.h>
#include "../InventoryComponent/TDAInventoryComponent.h"
#include "../Characters/TDACharacter.h"
#include "../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"

// Sets default values
AWeaponDefault::AWeaponDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("Shoot Location"));
	ShootLocation->SetupAttachment(RootComponent);

	DropShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("Drop Shoot Location"));
	DropShootLocation->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponInit();
}

// Called every frame
void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	ClipDropTick(DeltaTime);
	
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	if (GetWeaponRound() > 0 && WeaponFiring && !WeaponReloading)
	{
		if (FireTimer < 0.0f)
		{

			Fire();

		}
		else
		{
			FireTimer -= DeltaTime;
		}
		
	}
	
}

void AWeaponDefault::ReloadTick(float DeltaTime)
{
	if (WeaponReloading)
	{
		if (ReloadTimer < 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::ClipDropTick(float DeltaTime)
{
	if (DropClipFlag)
	{
		if (DropClipTimer < 0.0f)
		{
			DropClipFlag = false;
			InitDropMesh(WeaponSetting.ClipDropMesh.DropMesh, WeaponSetting.ClipDropMesh.DropMeshOffset, WeaponSetting.ClipDropMesh.DropMeshLifeTime);
		}
		else
		{
			DropClipTimer -= DeltaTime;
		}
	}
}


void AWeaponDefault::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}
	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent();
	}
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	if (CheckWeaponCanFire())
	{
		WeaponFiring = bIsFire;
	}
	else
	{
		WeaponFiring = false;
		FireTimer = 0.01f;
	}

}

bool AWeaponDefault::CheckWeaponCanFire()
{
	return true;
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

void AWeaponDefault::Fire()
{
	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponSetting.AnimWeaponInfo.AnimCharFire)
	{
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharFire;
	}

	FireTimer = WeaponSetting.RateOfFire;
	WeaponInfo.Round = WeaponInfo.Round - 1;

	OnWeaponFireStart.Broadcast(AnimToPlay);
	
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundFireWeapon, ShootLocation->GetComponentLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSetting.EffectFireWeapon, ShootLocation->GetComponentTransform());
	
	MuzzleLocation = ShootLocation->GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WeaponSetting.MuzzleFireFX, MuzzleLocation, GetActorRotation());


	int8 NumberProjectile = GetNumberProjectileByShot();

	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();

		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();

		FVector EndLocation;
		for (int8 i = 0; i < NumberProjectile; i++)//Shotgun
		{	
			EndLocation = GetFireEndLocation();
			
			if (ProjectileInfo.Projectile)
			{
				FVector Dir = EndLocation - SpawnLocation;
				Dir.Normalize();
				FMatrix myMatrix(Dir, FVector(0.0f, 1.0f, 0.0f), FVector(0.0f, 0.0f, 1.0f), FVector::ZeroVector);
				SpawnRotation = myMatrix.Rotator();

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				AProjectileDefault* myProjectile = Cast<AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myProjectile)
				{
					myProjectile->SetLifeSpan(WeaponSetting.ProjectileSetting.ProjectileInitSpeed);
					myProjectile->InitProjectile(WeaponSetting.ProjectileSetting);
					
				}

			}
			else
			{
				

			}
		}

		
	}
	if (GetWeaponRound() <= 0 && !WeaponReloading)
	{
		if (CheckCanWeaponReload())
		{
			InitReload();
		}
	}

}


int8 AWeaponDefault::GetNumberProjectileByShot() const
{
	return WeaponSetting.NumberProjectileByShot;
}

int32 AWeaponDefault::GetWeaponRound()
{
	return WeaponInfo.Round;
}

void AWeaponDefault::InitReload()
{
	WeaponReloading = true;

	ReloadTimer = WeaponSetting.ReloadTime;

	//ToDo Anim reload
	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponReloading) 
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharReload;

	if (WeaponSetting.ClipDropMesh.DropMesh)
	{
		DropClipFlag = true;
		DropClipTimer = WeaponSetting.ClipDropMesh.DropMeshTime;
	}

	OnWeaponReloadStart.Broadcast(AnimToPlay);
}

void AWeaponDefault::FinishReload()
{
	WeaponReloading = false;
	

	int8 AviableAmmoFromInventory = GetAviableAmmoForReload();
	int8 AmmoNeedTakeFromInv;
	int8 NeedToReload = WeaponSetting.MaxRound - WeaponInfo.Round;
	UE_LOG(LogTemp, Warning, TEXT("AviableAmmoFromInventory = %i. AmmoNeedTakeFromInv = %i. NeedToReload = %i"), AviableAmmoFromInventory, AmmoNeedTakeFromInv, NeedToReload);
	if (NeedToReload > AviableAmmoFromInventory)
	{
		WeaponInfo.Round += AviableAmmoFromInventory;
		AmmoNeedTakeFromInv = AviableAmmoFromInventory;
		UE_LOG(LogTemp, Warning, TEXT("( > ) AdditionalWeaponInfo.Round = %i. AmmoNeedTakeFromInv = %i"), WeaponInfo.Round, AmmoNeedTakeFromInv);

	}
	else
	{
		WeaponInfo.Round += NeedToReload;
		AmmoNeedTakeFromInv = NeedToReload;
		UE_LOG(LogTemp, Warning, TEXT("( < ) AdditionalWeaponInfo.Round = %i. AmmoNeedTakeFromInv = %i"), WeaponInfo.Round, AmmoNeedTakeFromInv);

	}

	OnWeaponReloadEnd.Broadcast(true, -AmmoNeedTakeFromInv);
	UE_LOG(LogTemp, Warning, TEXT("OnWeaponReloadEnd.Broadcast(true, -AmmoNeedTakeFromInv) = %i"), -AmmoNeedTakeFromInv);

}

void AWeaponDefault::CancelReload()
{
	WeaponReloading = false;
	if (SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.15f);
	}

	OnWeaponReloadEnd.Broadcast(false, 0);
	DropClipFlag = false;
}

FVector AWeaponDefault::GetFireEndLocation() const
{
	FVector EndLocation = FVector(0.0f);

	FVector tmpV =(ShootLocation->GetComponentLocation() - ShootEndLocation);
	if (tmpV.Size() < SizeVectorToChangeShootDirectionLogic)
	{
		EndLocation = ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * 200.0f;
	}
	else
	{
		EndLocation = ShootEndLocation;
	}
	
	return EndLocation;
}

void AWeaponDefault::InitDropMesh(UStaticMesh* DropMesh, FTransform Offset, float LifeTimerMesh)
{
	if (DropMesh)
	{
		FVector SpawnLocation = DropShootLocation->GetComponentLocation();
		FRotator SpawnRotation = DropShootLocation->GetComponentRotation();
			
		FTransform Transform;

		Transform.SetLocation(SpawnLocation);
		Transform.SetScale3D(Offset.GetScale3D());
		Transform.SetRotation((GetActorRotation() + Offset.Rotator()).Quaternion());
			
		AStaticMeshActor* NewActor = nullptr;

		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		Param.Owner = this;

		NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Param);
		
		if (NewActor && NewActor->GetStaticMeshComponent())
		{
			NewActor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
			NewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
				
			NewActor->SetActorTickEnabled(false);
			//NewActor->InitialLifeSpan = 5.0f;
			NewActor->SetLifeSpan(LifeTimerMesh);
			
			NewActor->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
			NewActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
			NewActor->GetStaticMeshComponent()->SetStaticMesh(DropMesh);
				
				
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel11, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel12, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Block);
				

		}
	}
}

bool AWeaponDefault::CheckCanWeaponReload()
{
	bool result = true;
	if (GetOwner())
	{
		ATDACharacter* Player = StaticCast<ATDACharacter*>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		UTDAInventoryComponent* MyInv = Cast<UTDAInventoryComponent>(Player->InventoryComponent);
		if (MyInv)
		{
			int8 AviableAmmoForWeapon;
			if (!MyInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				result = false;
			}
		}
	}

	return result;
}

int8 AWeaponDefault::GetAviableAmmoForReload()
{
	int8 AviableAmmoForWeapon = WeaponSetting.MaxRound;

	if (GetOwner())
	{
		ATDACharacter* Player = StaticCast<ATDACharacter*>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		UTDAInventoryComponent* MyInv = Cast<UTDAInventoryComponent>(Player->InventoryComponent);
		if (MyInv)
		{
			if (MyInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				AviableAmmoForWeapon = AviableAmmoForWeapon;//?????
			}
			
		}
	}
	return AviableAmmoForWeapon;
}

