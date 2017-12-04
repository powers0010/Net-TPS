// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplosionEffect.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
AExplosionEffect::AExplosionEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	ExplosionEffect = nullptr;
}

// Called when the game starts or when spawned
void AExplosionEffect::BeginPlay()
{
	Super::BeginPlay();
	
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this , ExplosionEffect,GetActorLocation(),GetActorRotation());
	}
	Destroy();
}


