// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterImpactEffect.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


// Sets default values
AShooterImpactEffect::AShooterImpactEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


}

// Called when the game starts or when spawned
void AShooterImpactEffect::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(1.f);
}

void AShooterImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticle, GetActorLocation(), GetActorRotation());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}

