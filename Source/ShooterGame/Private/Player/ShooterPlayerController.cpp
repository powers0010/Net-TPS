// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterPlayerController.h"
#include "MyPlayerCameraManager.h"
#include "Runtime/Engine/Classes/GameFramework/GameUserSettings.h"

AShooterPlayerController::AShooterPlayerController()
{
	PlayerCameraManagerClass = AMyPlayerCameraManager::StaticClass();
}

void AShooterPlayerController::BeginPlay()
{
	SetFullScreen(true);
}

void AShooterPlayerController::SetFullScreen(bool isFullScreen)
{
	if (isFullScreen)
	{
		UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(EWindowMode::WindowedFullscreen);//»òÕßEWindowMode::Fullscreen  
	}
	else
	{
		UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(EWindowMode::Windowed);
		UGameUserSettings::GetGameUserSettings()->SetScreenResolution(FIntPoint(800	,600));

	}
	UGameUserSettings::GetGameUserSettings()->ApplyResolutionSettings(false);
}