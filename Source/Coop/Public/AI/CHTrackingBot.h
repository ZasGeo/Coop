// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "CHTrackingBot.generated.h"

UCLASS()
class COOP_API ACHTrackingBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACHTrackingBot();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION()
	void OnCheckNearbyBots();

	UFUNCTION()
	void OnRep_PowerLevel();

	UFUNCTION()
	void RefreshPass();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetNexPathPoint();

	UFUNCTION()
	void OnHealthChanged(class UCHHealthComponent* HealthComponent, float Health, float DeltaHealth,
							const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void SelfDestruct();

	UFUNCTION()
	void DamageSelf();



	

protected:

	UPROPERTY(VisibleDefaultsOnly, Category = "Compontnts")
	class UStaticMeshComponent* MeshComp = nullptr;


	UPROPERTY(VisibleDefaultsOnly, Category = "Compontnts")
	class USphereComponent* SphereOverlap = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCHHealthComponent* HealthComp = nullptr;


	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	class UParticleSystem* ExplosionEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	class USoundCue* SelfDestructSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	class USoundCue* ExplosionSound = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float MovementForce = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceToTarget = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange = false;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ExplosionDamage = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ExplosionRadious = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float SelfDamageInterval = 0.5f;

	FVector NextPathPoint;

	class UMaterialInstanceDynamic* MatInst = nullptr;

	bool bExplode = false;

	bool bStartedSelfDestruction = false;

	FTimerHandle TimerHandle_SelfDamage;

	UPROPERTY(ReplicatedUsing = OnRep_PowerLevel, BlueprintReadOnly, Category = "Damage")
	int PowerLevel = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	int MaxPowerLevel = 3;

	FTimerHandle TimerHandle_PassTimer;
};
