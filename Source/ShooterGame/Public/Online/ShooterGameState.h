// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ShooterGameState.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Transient)	//临时数据，从
		int32 GameRemainTime;
	
	
};
