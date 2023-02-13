// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "../Weapons/WeaponDefault.h"
#include "../Types/Types.h"
#include "TDAGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TDA_API UTDAGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	//table
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " WeaponSetting ")
		UDataTable* WeaponInfoTable = nullptr;
	UFUNCTION(BlueprintCallable)
		bool GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo);
};
