// Fill out your copyright notice in the Description page of Project Settings.


#include "CHExplosiveBarrel.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "CHHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"

// Sets default values
ACHExplosiveBarrel::ACHExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	MeshComp->SetCollisionProfileName("PhysicsActor");
	MeshComp->SetSimulatePhysics(true);
	
	RootComponent = MeshComp;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("ForceComp"));
	RadialForceComp->AddCollisionChannelToAffect(ECollisionChannel::ECC_PhysicsBody);
	RadialForceComp->AddCollisionChannelToAffect(ECollisionChannel::ECC_Destructible);
	RadialForceComp->SetupAttachment(RootComponent);
	RadialForceComp->SetAutoActivate(false);

	HealthComp = CreateDefaultSubobject<UCHHealthComponent>(TEXT("HealthCOmp"));

	HealthComp->OnChangeHealth.AddDynamic(this, &ACHExplosiveBarrel::OnHealthChanged);
		
	SetReplicates(true);
	SetReplicateMovement(true);
	MeshComp->SetIsReplicated(true);
}



void ACHExplosiveBarrel::OnHealthChanged(UCHHealthComponent* HealthComponent, float Health, float DeltaHealth, const UDamageType* CausedDamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	
	if (Health <= 0.0f)
	{
		HealthComp->OnChangeHealth.RemoveDynamic(this, &ACHExplosiveBarrel::OnHealthChanged);

		if (MeshComp)
		{
			

			bIsDestroyed = true;
			
			PlayDestroyEffects();

			MeshComp->AddImpulse(GetActorUpVector() * 100000.0f);
		}

		

		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(this, 500.0f, GetActorLocation(), 500.0f,DamageType, IgnoredActors, this, InstigatedBy, true, ECollisionChannel::ECC_Visibility);
		DrawDebugSphere(GetWorld(), GetActorLocation(), 500.0f, 12, FColor::Red, false, 1.0f, 0, 1.0f);
		RadialForceComp->FireImpulse();

	}
}

void ACHExplosiveBarrel::PlayDestroyEffects()
{
	if (DestroyedMaterial && bIsDestroyed)
	{
		MeshComp->SetMaterial(0, DestroyedMaterial);
	}

	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation());
}

void ACHExplosiveBarrel::OnRep_DestroyEffects()
{
	PlayDestroyEffects();
}

void ACHExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACHExplosiveBarrel, bIsDestroyed);
	
}