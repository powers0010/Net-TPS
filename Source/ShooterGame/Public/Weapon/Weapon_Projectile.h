// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "Weapon_Projectile.generated.h"


USTRUCT()
struct FProjectileWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AShooterProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		int32 ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		int32 ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<UDamageType> DamageType;
};


UCLASS()
class SHOOTERGAME_API AWeapon_Projectile : public AWeapon
{
	GENERATED_BODY()
	
public:
	//开火
	void StartFire();

	virtual void FireWeapon() override;

	//获取子弹信息
	void ApplyWeaponConfig(FProjectileWeaponData& ProjectileData);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireProjectile(FVector Origin, FVector_NetQuantizeNormal ShooterDir);
protected:
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		FProjectileWeaponData ProjectileConfig;


};
