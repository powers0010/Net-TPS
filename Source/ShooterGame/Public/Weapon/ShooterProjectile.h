// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon_Projectile.h"
#include "ShooterProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;

UCLASS()
class SHOOTERGAME_API AShooterProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShooterProjectile();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	virtual void PostNetReceiveVelocity(const FVector& NewVelocity);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(category = Projectile , VisibleAnywhere, BlueprintReadOnly ,meta=(AllowPrivateAccess = "Collision"))
		USphereComponent* CollisionComp;

	UPROPERTY(category = Projectile, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "Projectile"))
		UProjectileMovementComponent* ProjectileMovementComp;

	UPROPERTY(category = Particle, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "Particle"))
		UParticleSystemComponent* ParticleSystemCom;

	UPROPERTY(category = Effect, EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<class AExplosionEffect> ExplosionEffectClass;


	virtual void PostInitializeComponents() override;

	//爆炸绑定函数
	UFUNCTION()
	void OnImpact(const FHitResult& ImpactResult);

	//
	void Explode(const FHitResult& ImpackResult);

	//炮弹信息
	struct FProjectileWeaponData WeaponConfig;

	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_Exploded)
		bool bExploded;

	UFUNCTION()
	void OnRep_Exploded();
public:	
	void InitializeVelocity(FVector ShooterDirection);
	
	TWeakObjectPtr<AController> MyController;
};
