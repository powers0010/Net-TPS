// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosionEffect.generated.h"


UCLASS()
class SHOOTERGAME_API AExplosionEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosionEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//±¬Õ¨ÌØÐ§
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	class UParticleSystem* ExplosionEffect;

public:	
	
	
};
