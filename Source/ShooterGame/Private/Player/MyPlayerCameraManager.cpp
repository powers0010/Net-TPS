// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerCameraManager.h"
#include "ShooterCharacter.h"



void AMyPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);

	AShooterCharacter* MyPlayer = Cast<AShooterCharacter>(PCOwner ? PCOwner->GetPawn() : nullptr);
	if (MyPlayer)
	{
		MyPlayer->OnCameraUpdata(GetCameraLocation(), GetCameraRotation());
	}

}
