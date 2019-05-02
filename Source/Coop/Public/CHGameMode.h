// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CHGameMode.generated.h"


enum class EWaveState : uint8;




DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnKillActorSignature, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);


/**
 * 
 */
UCLASS()
class COOP_API ACHGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ACHGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Gamemode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	void StartWave();

	void EndWave();

	void PrepareForNextWave();

	void CheckWaweState();

	void CheckAnyPlayerAlive();

	void GameOver();

	void SetGameState(EWaveState NewState);

	void RestartDeadPlayers();

public:

	UPROPERTY(BlueprintAssignable, Category = "Gamemode")
	FOnKillActorSignature OnActorKilled;

protected:

	FTimerHandle Timer_SpawnWawe;

	FTimerHandle Timer_NextWaweStart;

	int32 NumberOfBotsToSpawn;

	int32 WaveCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Gamemode")
	float TimeBetweenWaves = 2.0f;



};
