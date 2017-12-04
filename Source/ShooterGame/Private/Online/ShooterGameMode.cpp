// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGameMode.h"
#include "ShooterCharacter.h"
#include "Player/ShooterPlayerController.h"
#include "Player/ShooterPlayerState.h"
#include "Online/ShooterGameState.h"
#include "Player/ShooterSpectatorPawn.h"
#include "UI/ShooterHUD.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "ShooterAIController.h"




AShooterGameMode::AShooterGameMode()
{
	// 	//≈‰÷√ª˘¥°øÚº‹¿‡
 	static ConstructorHelpers::FClassFinder<AShooterCharacter> BP_ShootCharacter(TEXT("/Game/Blueprints/Charactor/BP_ShooterCharacter"));
 	DefaultPawnClass = BP_ShootCharacter.Class;

	PlayerControllerClass = AShooterPlayerController::StaticClass();
 	PlayerStateClass = AShooterPlayerState::StaticClass();
	GameStateClass = AShooterGameState::StaticClass();
	SpectatorClass = AShooterSpectatorPawn::StaticClass();
 	HUDClass = AShooterHUD::StaticClass();
}


void AShooterGameMode::StartPlay()
{
	Super::StartPlay();

//	CreateBotController();

//	StartBots();
}

void  AShooterGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	GetWorldTimerManager().SetTimer(SampleTimerHandle,this, &AShooterGameMode::DefaultTimer, 1.f, true);
}

void  AShooterGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(GameState);
	if (ShooterGameState)
	{
		ShooterGameState->GameRemainTime = GameRoundTime;
	}
}


AShooterAIController* AShooterGameMode::CreateBotController()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.OverrideLevel = nullptr;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AShooterAIController* BotController = GetWorld()->SpawnActor<AShooterAIController>(SpawnInfo);
	return BotController;
}

void AShooterGameMode::StartBots()
{
	UWorld* world = GetWorld();
	for (FConstControllerIterator It = world->GetControllerIterator();It;It++)
	{
		AShooterAIController* BotController = Cast<AShooterAIController>(*It);
		if (BotController)
		{
			RestartPlayer(BotController);
		}
	}
}

void AShooterGameMode::DefaultTimer()
{
	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(GameState);
	if (ShooterGameState && ShooterGameState->GameRemainTime>0)
	{
		ShooterGameState->GameRemainTime--;
//		UE_LOG(LogTemp, Warning, TEXT("Health: %d"), ShooterGameState->GameRemainTime);
		if (ShooterGameState->GameRemainTime<=0)
		{
			RestartGame();
		}
	}
}