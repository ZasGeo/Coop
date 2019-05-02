// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CHHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnChangeHealthSugnature, class UCHHealthComponent*, HealthComp, float, Health, float, DeltaHealth,
												const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOP_API UCHHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCHHealthComponent();

	UFUNCTION(BlueprintCallable, Category = "Events")
	void Heal(float Amount);

	float GetHealth() const 
	{
		return Health;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDamageTaken(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnRep_Health(float OldHealth);

protected:

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Heath")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heath")
	float Defaulthealth = 100.f;

	bool bIsDead = false;

public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnChangeHealthSugnature OnChangeHealth;
		
};
