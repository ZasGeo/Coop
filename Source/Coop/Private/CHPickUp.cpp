// Fill out your copyright notice in the Description page of Project Settings.


#include "CHPickUp.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "CHPowerUpActor.h"
#include "TimerManager.h"

// Sets default values
ACHPickUp::ACHPickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;
	   
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetupAttachment(RootComponent);
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ACHPickUp::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		Respawn();
	}

	
}

void ACHPickUp::Respawn()
{
	if (PowerUpClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerUpClass is nullptr in %s"), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	PowerUpInstance = GetWorld()->SpawnActor<ACHPowerUpActor>(PowerUpClass, GetTransform(), SpawnParams);
}

void ACHPickUp::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority && PowerUpInstance)
	{
		PowerUpInstance->ActivatePowerup(OtherActor);
		PowerUpInstance = nullptr;

		
		GetWorldTimerManager().SetTimer(Timer_Respawm, this,  &ACHPickUp::Respawn, CoolDownDuration);
	}


}


