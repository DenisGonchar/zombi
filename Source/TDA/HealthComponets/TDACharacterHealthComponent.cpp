// Fill out your copyright notice in the Description page of Project Settings.


#include "TDACharacterHealthComponent.h"

void UTDACharacterHealthComponent::ChangeHealthValue(float ChangeValue)
{
	float CurrentDamage = ChangeValue * CoefDamage;

	if (Shield > 0.0f && ChangeValue < 0.0f)
	{
		ChangeShieldValue(ChangeValue);

	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CollDownShieldTimer);

		if (DeadShield && Shield < 0.0f)
		{
			UE_LOG(LogTemp, Error, TEXT("Sheld < 0 "));
			OnShieldEffect.Broadcast();
			DeadShield = false;
		}

		Super::ChangeHealthValue(ChangeValue);

		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_CollDownShieldTimer, this, &UTDACharacterHealthComponent::CoolDownShieldEnd, CoolDownShieldRecoveryTime, false);
		}
	}

}

float UTDACharacterHealthComponent::GetCurrentShield()
{
	return Shield;
}

void UTDACharacterHealthComponent::ChangeShieldValue(float ChangeValue)
{
	DeadShield = true;

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CollDownShieldTimer);

	Shield = FMath::Clamp(Shield + ChangeValue, 0.0f, ShieldMax);

	OnChieldChange.Broadcast(Shield, ChangeValue);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CollDownShieldTimer, this, &UTDACharacterHealthComponent::CoolDownShieldEnd, CoolDownShieldRecoveryTime, false);

		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecoveryRateTimer);
	}

	OnChieldChange.Broadcast(Shield, ChangeValue);

	UE_LOG(LogTemp, Error, TEXT("UTPSCharacterHeaithComponent::ChangeShieldValue - Sheld > 0 "));
	OnShieldEffect.Broadcast();
}

void UTDACharacterHealthComponent::CoolDownShieldEnd()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShieldRecoveryRateTimer, this, &UTDACharacterHealthComponent::RecoveryShield, ShieldRecoveryRate, true);
	}
}

void UTDACharacterHealthComponent::RecoveryShield()
{
	Shield = FMath::Min(Shield + ShieldRecoveryValue, ShieldMax);

	if (Shield == ShieldMax && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecoveryRateTimer);
	}

	OnChieldChange.Broadcast(Shield, ShieldRecoveryValue);
}
