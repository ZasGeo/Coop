// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHExplosiveBarrel.generated.h"

UCLASS()
class COOP_API ACHExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHExplosiveBarrel();

protected:

	UFUNCTION()
	void OnHealthChanged(class UCHHealthComponent* HealthComponent, float Health, float DeltaHealth, const class UDamageType* CausedDamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class URadialForceComponent* RadialForceComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCHHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	class UMaterialInterface* DestroyedMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	TSubclassOf<UDamageType> DamageType;
};
