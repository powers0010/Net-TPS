// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon_Projectile.h"
#include "ShooterProjectile.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


void AWeapon_Projectile::FireWeapon()
{
	//��ʼ���ӵ����� �� ��ʼλ��
	const FVector ShootDirection = GetAdjustAim();
	const FVector Origin = GetMuzzleLocation();

	ServerFireProjectile(Origin, ShootDirection);
}

bool AWeapon_Projectile::ServerFireProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal ShooterDir)
{
	return true;
}

void AWeapon_Projectile::ServerFireProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal ShooterDir)
{
	FTransform SpawnTransform(ShooterDir.Rotation(), Origin);

	//��ʱ�����ӵ����������ӵ�����
	AShooterProjectile* Projectile = Cast<AShooterProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTransform));
	if (Projectile)
	{
		Projectile->Instigator = this->Instigator;	//���´���Instigator
		Projectile->SetOwner(this);
		Projectile->InitializeVelocity(ShooterDir);
		//�����ʱ����
		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);
	}
}


void AWeapon_Projectile::ApplyWeaponConfig(FProjectileWeaponData& ProjectileData)
{
	ProjectileData = ProjectileConfig;
}