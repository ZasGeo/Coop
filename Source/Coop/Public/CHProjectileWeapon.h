// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CHWeapon.h"
#include "CHProjectileWeapon.generated.h"

class ACHProjectile;

UCLASS()
class COOP_API ACHProjectileWeapon : public ACHWeapon
{
	GENERATED_BODY()

protected:

	virtual void Fire() override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	TSubclassOf<ACHProjectile> ProjectileClass;
	
};
