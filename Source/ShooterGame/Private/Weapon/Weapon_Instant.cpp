// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon_Instant.h"
#include "ShooterCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "ShooterImpactEffect.h"
#include "Net/UnrealNetwork.h"
#include "ShooterGame.h"

void AWeapon_Instant::FireWeapon()
{
	//计算子弹发射方向
// 	const int32 RandomSeed = FMath::Rand();
// 	FRandomStream WeaponRandomStream(RandomSeed);
// 
// 	const float CurrentSpread = InstantWeaponConfig.WeaponSpreed;
// 	const float CurrentHalfAngle = FMath::DegreesToRadians(CurrentSpread / 2.f);
	const FVector AimDir = GetAdjustAim();

//	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir,0,0/*CurrentHalfAngle, CurrentHalfAngle*/);

	//伤害对象
	const FVector StartLocation = GetMuzzleLocation();
	const float DamageRange = WeaponConfig.WeaponRange;

//	const FHitResult HitResult = WeaponTrace(StartLocation, StartLocation + DamageRange*AimDir/*ShootDir*/);
	const FHitResult HitResult = PawnOwner->TraceForCrossHair(DamageRange, COLLISION_WEAPON);

	//伤害处理
	ProcessInstantHit(HitResult, StartLocation, AimDir/*, RandomSeed, CurrentSpread*/);
//	UE_LOG(LogTemp, Warning, TEXT("Fire Weapon"));
}

void AWeapon_Instant::ProcessInstantHit(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir/*, int32 RandomSeed, float ReticleSpreed*/)
{
	if (PawnOwner&& PawnOwner->IsLocallyControlled() && GetNetMode()==NM_Client)
	{
		if (Impact.GetActor()/* && Impact.Actor->GetRemoteRole()==ROLE_Authority*/)
		{
			ServerNotifyHit(Impact, ShootDir);
		}
		else if(!Impact.GetActor())
		{
			if (Impact.bBlockingHit )
			{
				ServerNotifyHit(Impact, ShootDir);
			}
			else
			{
				ServerNotifyMiss(Impact, ShootDir);
			}
		}
	}
	ProcessInstantHit_Confirm(Impact, Orign, ShootDir/*, RandomSeed, ReticleSpreed*/);
}

bool AWeapon_Instant::ServerNotifyHit_Validate(FHitResult Impact, FVector ShootDir)
{
	return true;
}

void AWeapon_Instant::ServerNotifyHit_Implementation(FHitResult Impact, FVector ShootDir)
{
	FVector Origin = GetMuzzleLocation();
	if (Instigator && (Impact.GetActor()||Impact.bBlockingHit))
	{
		if (Impact.GetActor()== nullptr && Impact.bBlockingHit)
		{
			ProcessInstantHit_Confirm(Impact, Origin, ShootDir);
		}
		else
		{
			ProcessInstantHit_Confirm(Impact, Origin, ShootDir);
		}
	}
//	UE_LOG(LogTemp, Warning, TEXT("ServerNotifyHit"));

}
bool AWeapon_Instant::ServerNotifyMiss_Validate(FHitResult Impact, FVector ShootDir)
{
	return true;
}

void AWeapon_Instant::ServerNotifyMiss_Implementation(FHitResult Impact, FVector ShootDir)
{
//	UE_LOG(LogTemp, Warning, TEXT("ServerNotifyMiss"));
}
void AWeapon_Instant::ProcessInstantHit_Confirm(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir/*, int32 RandomSeed, float ReticleSpreed*/)
{
	//生命值处理
	if (Impact.GetActor() && ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}
	
	if (Role == ROLE_Authority)
	{
//		HitNotify.HitNum = HitNotify.HitNum++ % 20;
		HitNotify.Origin = Orign;
		HitNotify.Impact = Impact;
		HitNotify.ShooterDir = ShootDir;
	}

	if (GetNetMode() != NM_DedicatedServer/*PawnOwner&& PawnOwner->IsLocallyControlled()*/)
	{
		//生成开枪特效
		const FVector EndTrace = Orign + ShootDir*WeaponConfig.WeaponRange;
		const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
		//	UE_LOG(LogTemp, Warning, TEXT("EndPoint : %s"), *EndPoint.ToString());
		SpawnTraceEffect(EndPoint);
		//生成击中特效
		SpawnImpactEffects(Impact);
	}
}

// void AWeapon_Instant::SimulateWeaponFire()
// {
// 	Super::SimulateWeaponFire();
// 	if (!(GetNetMode()== NM_DedicatedServer|| PawnOwner->IsLocallyControlled()))
// 	{
// 		SimulateInstantHit(HitNotify.Origin, HitNotify.Impact, HitNotify.ShooterDir);
// 	}
// }

void AWeapon_Instant::SimulateInstantHit(const FVector& Origin, const FHitResult& Impact, const FVector& ShooterDir)
{
	if (Impact.bBlockingHit)
	{
		SpawnImpactEffects(Impact);
		SpawnTraceEffect(Impact.ImpactPoint);
	}
	else
	{
		const FVector EndLoc = Origin + WeaponConfig.WeaponRange*ShooterDir;
		SpawnTraceEffect(EndLoc);
	}
//	UE_LOG(LogTemp, Warning, TEXT("SimulateInstantHit"));
}

void AWeapon_Instant::SpawnTraceEffect(const FVector& EndPoint)
{
	if (TrailEffect)
	{
		const FVector Orign = GetMuzzleLocation();
		UParticleSystemComponent* TralEmitter = UGameplayStatics::SpawnEmitterAtLocation(this, TrailEffect, Orign);
		if (TralEmitter)
		{
			TralEmitter->SetVectorParameter(TrailTargetParam , EndPoint);

//			UE_LOG(LogTemp, Warning, TEXT("TralEmitter"));
		}
	}
}

void AWeapon_Instant::SpawnImpactEffects(const FHitResult& Impact)
{
	if (ImpactEffectActor && Impact.bBlockingHit)
	{
		const FTransform SpawnTrans(Impact.Normal.Rotation(), Impact.ImpactPoint);
		AShooterImpactEffect* ImpactEffect = GetWorld()->SpawnActorDeferred<AShooterImpactEffect>(ImpactEffectActor, SpawnTrans);
		if (ImpactEffect)
		{
			UGameplayStatics::FinishSpawningActor(ImpactEffect, SpawnTrans);
//			UE_LOG(LogTemp, Warning, TEXT("FinishSpawningActor ImpactEffect"));
		}
	}
}

void AWeapon_Instant::OnRep_HitNotify()
{
//	UE_LOG(LogTemp, Warning, TEXT("OnRep_HitNotify"));
	SimulateInstantHit(HitNotify.Origin, HitNotify.Impact,HitNotify.ShooterDir);
}

void AWeapon_Instant::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AWeapon_Instant, HitNotify, COND_SkipOwner);
}