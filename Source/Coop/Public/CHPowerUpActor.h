// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHPowerUpActor.generated.h"

UCLASS()
class COOP_API ACHPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHPowerUpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTickPowerup();



public:	
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated(AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();

	void ActivatePowerup(AActor* OtherActor);

	UFUNCTION()
	void OnRep_PowerupActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerUpStateChanged(bool bNewIsActive);

protected:

	/*Time between power up ticks*/
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerUpInterval = 0.0f;

	/*Total times we apply the power up effect*/
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 NumberOfTicks = 0;

	FTimerHandle TimerHandle_PowerUpTick;

	int32 TickProcessed = 0;

	UPROPERTY(ReplicatedUsing = OnRep_PowerupActive)
	bool bIsPowerupActive = false;
};
