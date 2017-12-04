// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "Weapon_Machine.generated.h"

/**
 * 
 */
class AShooterImpactEffect;

UCLASS()
class SHOOTERGAME_API AWeapon_Machine : public AWeapon
{
	GENERATED_BODY()
public:
	AWeapon_Machine();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void HandleStartFire() override;

	virtual void HandleEndFire() override;

	virtual void HandleFiring() override;

	virtual void FireWeapon() override;

//	virtual void SimulateWeaponFire() override;

	bool CanLoop();

	void ProcessMachineHit(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir/*, int32 RandomSeed, float ReticleSpreed*/);

	void ProcessMachineHit_Confirm(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir/*, int32 RandomSeed, float ReticleSpreed*/);

	//	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

	void SimulateMachineHit(const FVector& Origin, const FHitResult& Impact, const FVector& ShooterDir);

	void SpawnImpactEffects(const FHitResult& Impact);

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerNotifyHit(FHitResult Impact, FVector Origin);

	UFUNCTION(Unreliable, Server, WithValidation)
		void ServerNotifyMiss(FHitResult Impact, FVector Origin);

	UFUNCTION(Unreliable, NetMulticast, WithValidation)
		void MultiSimulateLoop(bool bsifiring);

	UFUNCTION()
		void OnRep_HitNotify();
	
protected:
	UPROPERTY(EditDefaultsOnly, category = Impact)
		TSubclassOf<AShooterImpactEffect> ImpactEffectActor;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitNotify)
		FImpactHitInfo HitNotify;

	FTimerHandle Timer_OnStartFire;

	UPROPERTY(Category = Weapon, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UAudioComponent* FireAudioCom;

	UPROPERTY(Category = Weapon, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* FireParticleCom;

};
