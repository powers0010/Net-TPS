// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"


void UShooterAnimInstance::UpdataVariable()
{
	AShooterCharacter* OwnerPlayer = Cast<AShooterCharacter>(TryGetPawnOwner());
	if (OwnerPlayer)
	{
		//bisInAir
		if (OwnerPlayer->GetMovementComponent())
		{
			bIsInAir = OwnerPlayer->GetMovementComponent()->IsFalling();
		}
		//Speed And Direction
		Speed = OwnerPlayer->GetVelocity().Size();
		if (Speed > 10.f)
		{
			FRotator SpeedInWorld = OwnerPlayer->GetVelocity().Rotation();
			FRotator WorldToLocal = OwnerPlayer->GetActorRotation().GetInverse();
			FRotator SpeedInLocal = UKismetMathLibrary::ComposeRotators(WorldToLocal, SpeedInWorld);
			Direction = SpeedInLocal.Yaw > 180.f ? SpeedInLocal.Yaw - 360.f : SpeedInLocal.Yaw;
		}
		else
		{
			Direction = 0.f;
		}
		//AimOffset
		FRotator AimOffset = OwnerPlayer->GetAimOffsets();
		Yaw = AimOffset.Yaw / 180.f;
		Pitch = AimOffset.Pitch / 180.f;
	}
}