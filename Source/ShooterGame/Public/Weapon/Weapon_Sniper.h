// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "Weapon_Sniper.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AWeapon_Sniper : public AWeapon
{
	GENERATED_BODY()

public:
	AWeapon_Sniper();

	void AttachWeaponToPawn();

	void DetachWeaponFromPawn();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void FireWeapon() override;

	virtual void OnStartTarget();

	virtual void OnStopTarget();

	void SpawnImpactEffects(const FHitResult& Impact);

	void ProcessSniperHit(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir);

	void ProcessSniprHit_Confirm(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir);

//	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

	virtual bool CanFire() const;

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerNotifyHit(FHitResult Impact, FVector Origin);

	UFUNCTION(Unreliable, Server, WithValidation)
		void ServerNotifyMiss(FHitResult Impact, FVector Origin);

	UFUNCTION()
		void OnRep_HitNotify();
protected:
	UPROPERTY(EditDefaultsOnly, Category = FOV)
		float TargetFOV;

	UPROPERTY(EditDefaultsOnly, Category = FOV)
		float NormalFov;

	UPROPERTY(Category = Weapon, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* SniperViewMesh;		//Ãé×¼¾µ

	UPROPERTY(EditDefaultsOnly, category = Impact)
		TSubclassOf<class AShooterImpactEffect> ImpactEffectActor;
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitNotify)
		FImpactHitInfo HitNotify;

	float StoreCameraSpringArm;
	
};
