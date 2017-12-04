// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Aimoffset")
		void UpdataVariable();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "AimOffset")
		bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = "AimOffset")
		float Yaw;

	UPROPERTY(BlueprintReadOnly, Category = "AimOffset")
		float Pitch;

	UPROPERTY(BlueprintReadOnly, Category = "AimOffset")
		float Speed;

	UPROPERTY(BlueprintReadOnly, Category = "AimOffset")
		float Direction;
	
};
