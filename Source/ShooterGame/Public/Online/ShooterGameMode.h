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

	//��Ϸ���翪ʼʱ����
	void HandleMatchHasStarted() override;

	class AShooterAIController* CreateBotController();

	void StartBots();

	void DefaultTimer();
protected:

	FTimerHandle SampleTimerHandle;

	//һ����Ϸ����ʱ��
	UPROPERTY(Config)
		int32 GameRoundTime;
};
