// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHWeapon.generated.h"



USTRUCT()
struct FHitScanTrace{
	GENERATED_BODY()

public:

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;


	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class COOP_API ACHWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();


	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartReload();

protected:
	
	virtual void BeginPlay() override;

	void PlayFireEffect(const FVector& TracePoint);

	void PlayImpactEffect(EPhysicalSurface SurfaceType, const FVector& ImpactPoint);

	virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	void Reload();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TraceTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UParticleSystem* TraceEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage = 20.0f;

	FTimerHandle RepeatFire_Timer;

	float LastFireTime;

	/*Bullets per minute*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire = 100.0f;

	float TimeBetWeenShots;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float CurrentAmmo;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bIsReloading = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float MaxAmmo = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ReloadTime = 5.0f;

	/*Bullet spread in Degrees*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
	float BulletSpread = 3.0f;

	FTimerHandle Reload_Timer;


	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;


//Networking functions
protected:

	UFUNCTION()
	void OnRep_HitScanTrace();


};
