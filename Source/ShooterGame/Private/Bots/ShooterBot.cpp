// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterBot.h"
#include "ShooterAIController.h"


AShooterBot::AShooterBot()
{
	AIControllerClass = AShooterAIController::StaticClass();
}

