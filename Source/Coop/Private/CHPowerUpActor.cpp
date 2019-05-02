// Fill out your copyright notice in the Description page of Project Settings.


#include "CHPowerUpActor.h"
#include "UnrealNetwork.h"

// Sets default values
ACHPowerUpActor::ACHPowerUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);

}

// Called when the game starts or when spawned
void ACHPowerUpActor::BeginPlay()
{
	Super::BeginPlay();

	
	
}

void ACHPowerUpActor::OnTickPowerup()
{
	++TickProcessed;

	OnPowerupTicked();

	if (TickProcessed >= NumberOfTicks)
	{
		

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerUpTick);

		bIsPowerupActive = false;

		OnExpired();
	}


}

void ACHPowerUpActor::ActivatePowerup(AActor* OtherActor)
{
	OnActivated(OtherActor);

	bIsPowerupActive = true;
	OnRep_PowerupActive();

	if (PowerUpInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerUpTick, this, &ACHPowerUpActor::OnTickPowerup, PowerUpInterval, true);
	}

	else
	{
		OnTickPowerup();
	}
}

void ACHPowerUpActor::OnRep_PowerupActive()
{
	OnPowerUpStateChanged(bIsPowerupActive);
}

void ACHPowerUpActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACHPowerUpActor, bIsPowerupActive);
}