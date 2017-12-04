// Fill out your copyright notice in the Description page of Project Settings.	//及时伤害武器  锥形角

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "Weapon_Instant.generated.h"

/**
 * 
 */

class AShooterImpactEffect;


UCLASS()
class SHOOTERGAME_API AWeapon_Instant : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void FireWeapon() override;

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir/*, int32 RandomSeed, float ReticleSpreed*/);

	void ProcessInstantHit_Confirm(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir/*, int32 RandomSeed, float ReticleSpreed*/);

//	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

//	virtual void SimulateWeaponFire() override;

	void SimulateInstantHit(const FVector& Origin, const FHitResult& Impact, const FVector& ShooterDir);

	void SpawnTraceEffect(const FVector& EndPoint );

	void SpawnImpactEffects(const FHitResult& Impact);

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerNotifyHit(FHitResult Impact, FVector Origin);

	UFUNCTION(Unreliable, Server, WithValidation)
		void ServerNotifyMiss(FHitResult Impact, FVector Origin);

	UFUNCTION()
		void OnRep_HitNotify();
protected:
	UPROPERTY(EditDefaultsOnly, category = Particles)
		FName TrailTargetParam;		//特效目标设置 名称

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		UParticleSystem* TrailEffect;


	UPROPERTY(EditDefaultsOnly, category = Impact)
		TSubclassOf<AShooterImpactEffect> ImpactEffectActor;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitNotify)
		FImpactHitInfo HitNotify;
};
