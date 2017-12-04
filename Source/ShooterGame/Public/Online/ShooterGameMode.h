// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "ShooterGameMode.generated.h"



/**
 * 
 */

UCLASS()
class SHOOTERGAME_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AShooterGameMode();
	//
	void StartPlay() override;

	void PreInitializeComponents() override;

	//游戏世界开始时调用
	void HandleMatchHasStarted() override;

	class AShooterAIController* CreateBotController();

	void StartBots();

	void DefaultTimer();
protected:

	FTimerHandle SampleTimerHandle;

	//一局游戏持续时间
	UPROPERTY(Config)
		int32 GameRoundTime;
};
