// Fill out your copyright notice in the Description page of Project Settings.


#include "CHTrackingBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "CHHealthComponent.h"
#include "CHCharacter.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "UnrealNetwork.h"

static int32 DebugTrackerDrawing = 0;

FAutoConsoleVariableRef CVARDebugTrackerDrawing(TEXT("COOP.DebugTrackerBot"), DebugTrackerDrawing, TEXT("Draw debug lines for TrackerBot"), ECVF_Cheat);

// Sets default values
ACHTrackingBot::ACHTrackingBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	SphereOverlap = CreateDefaultSubobject<USphereComponent>(TEXT("SphereOverlap"));
	SphereOverlap->SetSphereRadius(200.0f);
	SphereOverlap->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereOverlap->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereOverlap->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SphereOverlap->SetupAttachment(RootComponent);

	HealthComp = CreateDefaultSubobject<UCHHealthComponent>(TEXT("HalthComp"));
	HealthComp->OnChangeHealth.AddDynamic(this, &ACHTrackingBot::OnHealthChanged);

	

}

// Called when the game starts or when spawned
void ACHTrackingBot::BeginPlay()
{
	Super::BeginPlay();
	
	

	if (Role == ROLE_Authority)
	{
		NextPathPoint = GetNexPathPoint();

		FTimerHandle TimerHandle_CheckPowerLevel;
		GetWorldTimerManager().SetTimer(TimerHandle_CheckPowerLevel, this, &ACHTrackingBot::OnCheckNearbyBots, 1.0f, true);
	}

	
}

FVector ACHTrackingBot::GetNexPathPoint()
{
	AActor* BestTarget = nullptr;
	float NearestTargetDistance = FLT_MAX;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || UCHHealthComponent::IsFrendly(this, TestPawn))
		{
			continue;
		}

		UCHHealthComponent* OtherHealthComp = Cast<UCHHealthComponent>(TestPawn->GetComponentByClass(UCHHealthComponent::StaticClass()));

		if (OtherHealthComp && OtherHealthComp->GetHealth() > 0.0f)
		{
			float Distance = (GetActorLocation() - TestPawn->GetActorLocation()).Size();
			if (Distance < NearestTargetDistance)
			{
				NearestTargetDistance = Distance;
				BestTarget = TestPawn;
			}
		}
	}

	if (BestTarget)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), BestTarget);

		GetWorldTimerManager().ClearTimer(TimerHandle_PassTimer);

		GetWorldTimerManager().SetTimer(TimerHandle_PassTimer, this, &ACHTrackingBot::RefreshPass, 5.0f, false);

		if (NavPath->PathPoints.Num() > 1)
		{
			return NavPath->PathPoints[1];
		}
	}

	return GetActorLocation();
}



void ACHTrackingBot::OnHealthChanged(UCHHealthComponent* HealthComponent, float Health, float DeltaHealth,
	const  UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	if (!MatInst)
	{
		MatInst = MeshComp->CreateDynamicMaterialInstance(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
	

	if (Health <= 0.0f)
	{
		SelfDestruct();
	}
}

void ACHTrackingBot::SelfDestruct()
{

	if (bExplode)
	{
		return;
	}

	bExplode = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, GetActorLocation());

	if (DebugTrackerDrawing > 0)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadious, 12, FColor::Red, false, 1.0f, 0, 1.0f);
	}
	
	MeshComp->SetVisibility(false, true);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Role == ROLE_Authority)
	{
		float Damage = ExplosionDamage + ExplosionDamage * PowerLevel;

		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(this, Damage, GetActorLocation(), ExplosionRadious, nullptr, IgnoredActors, this, GetInstigatorController(), true, ECollisionChannel::ECC_Visibility);

		SetLifeSpan(2.0f);
	}

	
}

void ACHTrackingBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20.0f, GetInstigatorController(), this, nullptr);
}



// Called every frame
void ACHTrackingBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority && !bExplode)
	{
		FVector ActorLocation = GetActorLocation();

		FVector DirectionToTarget = NextPathPoint - ActorLocation;

		float DistanceToTarget = DirectionToTarget.Size();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNexPathPoint();

			if (DebugTrackerDrawing > 0)
			{
				DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!!!");
			}

			
		}
		else
		{
			DirectionToTarget.Normalize();
			DirectionToTarget *= MovementForce;
			MeshComp->AddForce(DirectionToTarget, NAME_None, bUseVelocityChange);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + DirectionToTarget, 32, FColor::Red, false, 0.0f, 0, 1.0f);
		}


		if (DebugTrackerDrawing > 0)
		{
			DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Red, false, 4.0f, 1.0f);
		}
		
	}

	

}

void ACHTrackingBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bStartedSelfDestruction && bExplode)
	{
		return;
	}

	ACHCharacter* PlayerPawn = Cast<ACHCharacter>(OtherActor);

	if (PlayerPawn && !UCHHealthComponent::IsFrendly(OtherActor, this))
	{
		if (Role == ROLE_Authority)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ACHTrackingBot::DamageSelf, SelfDamageInterval, true, 0.0f);
		}
		
		bStartedSelfDestruction = true;

		UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
	}
}


void ACHTrackingBot::OnCheckNearbyBots()
{
	const float Radius = 600.0f;

	FCollisionShape CollShape;
	CollShape.SetSphere(Radius);

	FCollisionObjectQueryParams QueryParams;

	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

	int32 NumOfBots = 0;

	for (FOverlapResult Result : Overlaps)
	{
		ACHTrackingBot* Bot = Cast<ACHTrackingBot>(Result.GetActor());

		if (Bot && Bot != this)
		{
			++NumOfBots;
		}
	}
	
	PowerLevel = FMath::Clamp(NumOfBots, 0, MaxPowerLevel);

	OnRep_PowerLevel();
}

void ACHTrackingBot::OnRep_PowerLevel()
{
	if (!MatInst)
	{
		MatInst = MeshComp->CreateDynamicMaterialInstance(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{

		float Alpha = (float)PowerLevel / (float)MaxPowerLevel;

		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}
}

void ACHTrackingBot::RefreshPass()
{
	NextPathPoint = GetNexPathPoint();
}

void ACHTrackingBot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACHTrackingBot, PowerLevel);
	
}