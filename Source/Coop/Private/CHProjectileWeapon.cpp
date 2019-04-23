// Fill out your copyright notice in the Description page of Project Settings.


#include "CHProjectileWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "CHProjectile.h"

void ACHProjectileWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	

	if (MyOwner && ProjectileClass!=nullptr)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
				
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Owner = MyOwner;
		ActorSpawnParams.Instigator = Instigator;
		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<ACHProjectile>(ProjectileClass, MuzzleLocation, EyeRotation, ActorSpawnParams);


	}

}
