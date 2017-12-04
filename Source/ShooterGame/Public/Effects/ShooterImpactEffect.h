// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterImpactEffect.generated.h"


class UParticleSystem;
class USoundCue;

UCLASS()
class SHOOTERGAME_API AShooterImpactEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShooterImpactEffect();

	virtual void PostInitializeComponents() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = default)
	UParticleSystem* ImpactParticle;

	//¿ªÇ¹ÒôÐ§
	UPROPERTY(EditDefaultsOnly, Category = default)
	USoundCue* ImpactSound;

public:	


	
	
};
