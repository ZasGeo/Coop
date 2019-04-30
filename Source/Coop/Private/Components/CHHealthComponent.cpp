// Fill out your copyright notice in the Description page of Project Settings.


#include "CHHealthComponent.h"
#include "GameFramework/DamageType.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UCHHealthComponent::UCHHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
	// ...
}


// Called when the game starts
void UCHHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UCHHealthComponent::OnDamageTaken);
		}
	}
	

	Health = Defaulthealth;
}

void UCHHealthComponent::OnDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, Defaulthealth);

	//UE_LOG(LogTemp, Log, TEXT("Health changed %s"), *FString::SanitizeFloat(Health));

	OnChangeHealth.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}



void UCHHealthComponent::OnRep_Health(float OldHealth)
{
	OnChangeHealth.Broadcast(this, Health, OldHealth - Health, nullptr, nullptr, nullptr);
}

void UCHHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCHHealthComponent, Health);
}