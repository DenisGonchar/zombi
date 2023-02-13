// Fill out your copyright notice in the Description page of Project Settings.


#include "TDAHealthComponent.h"

// Sets default values for this component's properties
UTDAHealthComponent::UTDAHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTDAHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTDAHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UTDAHealthComponent::GetCurrentHealth()
{
	return Health;
}



void UTDAHealthComponent::ChangeHealthValue(float ChangeValue)
{
	ChangeValue = ChangeValue * CoefDamage;

	Health += ChangeValue;

	if (Health > 100.0f)
	{
		Health = 100.0f;
	}
	else
	{

		if (Health <= 0.0f)
		{
			OnDead.Broadcast();

		}
	}

	OnHealthChange.Broadcast(Health, ChangeValue);
}

void UTDAHealthComponent::SetCurrentHealth(float NewHealth)
{
	Health = NewHealth;
}

