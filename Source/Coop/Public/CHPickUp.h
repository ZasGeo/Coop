// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHPickUp.generated.h"

UCLASS()
class COOP_API ACHPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void  Respawn();

public:	
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UDecalComponent* DecalComp;

	UPROPERTY(EditInstanceOnly, Category = "PickUpActor")
	TSubclassOf<class ACHPowerUpActor> PowerUpClass;

	UPROPERTY(EditInstanceOnly, Category = "PickUpActor")
	float CoolDownDuration = 10.0f;

	ACHPowerUpActor* PowerUpInstance;

	FTimerHandle Timer_Respawm;
};
