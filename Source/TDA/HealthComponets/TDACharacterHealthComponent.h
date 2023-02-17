// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDAHealthComponent.h"
#include "TDACharacterHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChieldChange, float, Shield, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChieldEffect);

UCLASS()
class TDA_API UTDACharacterHealthComponent : public UTDAHealthComponent
{
	GENERATED_BODY()
public:

	FTimerHandle TimerHandle_CollDownShieldTimer;

	FTimerHandle TimerHandle_ShieldRecoveryRateTimer;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
	FOnChieldChange OnChieldChange;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
	FOnChieldEffect OnShieldEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float CoolDownShieldRecoveryTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoveryValue = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoveryRate = 0.1f;

	virtual void ChangeHealthValue(float ChangeValue) override;


	UFUNCTION(BlueprintCallable, Category = "Shield")
	float GetCurrentShield();

	UFUNCTION(BlueprintCallable, Category = "Shield")
	void ChangeShieldValue(float ChangeValue);

	void CoolDownShieldEnd();

	void RecoveryShield();
protected:

	float Shield = 100.0f;
	float ShieldMax = 100.0f;

	bool DeadShield = true;
};
