// Fill out your copyright notice in the Description page of Project Settings.


#include "CHWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/EngineTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Coop.h"

static int32 DebugWeaponDrawing;

FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw debug lines for weapons"), ECVF_Cheat);

// Sets default values
ACHWeapon::ACHWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TraceTargetName = "Target";
}

void ACHWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetWeenShots = 60.0f / RateOfFire;

	CurrentAmmo = MaxAmmo;
}


void ACHWeapon::StartFire()
{

	float FirstDelay = FMath::Max(LastFireTime + TimeBetWeenShots - GetWorld()->TimeSeconds , 0.0f);

	GetWorldTimerManager().SetTimer(RepeatFire_Timer, this, &ACHWeapon::Fire, TimeBetWeenShots, true, FirstDelay);
}

void ACHWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(RepeatFire_Timer);
}

void ACHWeapon::StartReload()
{
	bIsReloading = true;
	GetWorldTimerManager().SetTimer(Reload_Timer, this, &ACHWeapon::Reload, ReloadTime, false);
}

void ACHWeapon::Fire()
{
	AActor* MyOwner = GetOwner();

	if (MyOwner && !bIsReloading)
	{

		if (CurrentAmmo <= 0)
		{
			StartReload();
			return;
		}

		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TracePoint = TraceEnd;

		FHitResult Hit;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECollisionChannel::COLLISION_WEAPON, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;

			if (SurfaceType == SURFACE_FLESH_VULNERABLE)
			{
				ActualDamage *= 4;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			
			UParticleSystem* SelectedEffect = nullptr;

			switch (SurfaceType)
			{
			case SURFACE_FLESH_DEFAULT:
				SelectedEffect = FleshImpactEffect;
				break;
			case SURFACE_FLESH_VULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

			TracePoint = Hit.ImpactPoint;

		}



		

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
		}
	
		PlayFireEffect(TracePoint);

		LastFireTime = GetWorld()->TimeSeconds;

		CurrentAmmo -= 1.0f;
	}

	
}



void ACHWeapon::Reload()
{
	bIsReloading = false;
	CurrentAmmo = MaxAmmo;
	GetWorldTimerManager().ClearTimer(Reload_Timer);
}

void ACHWeapon::PlayFireEffect(const FVector& TracePoint)
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);

	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	UParticleSystemComponent* TraceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);

	if (TraceComp)
	{
		TraceComp->SetVectorParameter(TraceTargetName, TracePoint);
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}


