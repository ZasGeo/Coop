// Fill out your copyright notice in the Description page of Project Settings.


#include "CHProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ACHProjectile::ACHProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComp->SetCollisionProfileName("BlockAll");

	RootComponent = CollisionComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionProfileName("NoCollision");

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	
	InitialLifeSpan = 2.0f;

	OnDestroyed.AddDynamic(this, &ACHProjectile::ACOnProjectileDestroyed);
}

// Called when the game starts or when spawned
void ACHProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACHProjectile::ACOnProjectileDestroyed(AActor* DestoyedActor)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyEffect, GetActorLocation());

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(this, 500.0f, GetActorLocation(), 500.0f, DamageType, IgnoredActors, this, GetInstigatorController(), true, ECollisionChannel::ECC_Visibility);
		DrawDebugSphere(GetWorld(), GetActorLocation(), 500.0f, 12, FColor::Red, false, 1.0f, 0, 1.0f);
	}
	
}

// Called every frame
void ACHProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

