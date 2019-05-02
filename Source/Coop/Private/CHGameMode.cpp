// Fill out your copyright notice in the Description page of Project Settings.


#include "CHGameMode.h"
#include "CHHealthComponent.h"
#include "CHGameState.h"
#include "CHPlayerState.h"

ACHGameMode::ACHGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	GameStateClass = ACHGameState::StaticClass();
	PlayerStateClass = ACHPlayerState::StaticClass();

}

void ACHGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void ACHGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaweState();
	CheckAnyPlayerAlive();
}

void ACHGameMode::StartWave()
{
	++WaveCount;

	NumberOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(Timer_SpawnWawe, this, &ACHGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);

	SetGameState(EWaveState::WaveInProgress);
}




void ACHGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(Timer_SpawnWawe);

	SetGameState(EWaveState::WaitingToComplete);
	
}

void ACHGameMode::PrepareForNextWave()
{
	RestartDeadPlayers();

	GetWorldTimerManager().SetTimer(Timer_NextWaweStart, this, &ACHGameMode::StartWave, TimeBetweenWaves, false);

	SetGameState(EWaveState::WaitingStart);

}

void ACHGameMode::CheckWaweState()
{

	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(Timer_NextWaweStart);

	if (NumberOfBotsToSpawn > 0 || bIsPreparingForWave) 
	{
		return;
	}

	bool bAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (!TestPawn || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		UCHHealthComponent* HealthComp = Cast<UCHHealthComponent>(TestPawn->GetComponentByClass(UCHHealthComponent::StaticClass()));

		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bAnyBotAlive = true;
			break;
		}
	}

	if (!bAnyBotAlive)
	{
		SetGameState(EWaveState::WaveComplete);

		PrepareForNextWave();
	}
}







void ACHGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* Pawn = PC->GetPawn();
			UCHHealthComponent* HealthComp = Cast<UCHHealthComponent>(Pawn->GetComponentByClass(UCHHealthComponent::StaticClass()));

			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
			{
				return;
			}
			
		}

	}

	GameOver();
	
}

void ACHGameMode::GameOver()
{
	EndWave();

	SetGameState(EWaveState::GameOver);

	UE_LOG(LogTemp, Log, TEXT("GAME OVER"));
}

void ACHGameMode::SetGameState(EWaveState NewState)
{
	ACHGameState* GS = GetGameState<ACHGameState>();
	if (ensureAlways(GS))
	{
		GS->SetGameState(NewState);
	}
}

void ACHGameMode::RestartDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}
		

	}
}

void ACHGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	--NumberOfBotsToSpawn;

	if (NumberOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}