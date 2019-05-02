// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CHGameState.generated.h"


UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingStart,

	WaveInProgress,

	WaitingToComplete,

	WaveComplete,

	GameOver,
};

/**
 * 
 */
UCLASS()
class COOP_API ACHGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	void SetGameState(EWaveState NewState);

protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChange(EWaveState NewState, EWaveState OldState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;

public:

	
	
};
