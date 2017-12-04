// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

		AShooterPlayerController();

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, category = "Settings")
		void SetFullScreen(bool isFullScreen);
};
