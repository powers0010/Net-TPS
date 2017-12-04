// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon_Machine.h"
#include "ShooterCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "ShooterImpactEffect.h"
#include "Net/UnrealNetwork.h"
#include "ShooterGame.h"


AWeapon_Machine::AWeapon_Machine() 
{
	FireAudioCom = CreateDefaultSubobject<UAudioComponent>(TEXT("FireAudioCom"));
	FireAudioCom->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	FireAudioCom->bAutoActivate = false;

	FireParticleCom = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticleCom"));
	FireParticleCom->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	FireParticleCom->bAutoActivate = false;

	bReplicates = true;
}

void AWeapon_Machine::HandleStartFire()
{
	if (CanFire())
	{
		HandleFiring();
		GetWorld()->GetTimerManager().SetTimer(Timer_OnStartFire, this, &AWeapon_Machine::HandleFiring, WeaponConfig.FireCD, true);
		if (Role == ROLE_Authority)
		{
			MultiSimulateLoop(true);
		}
	}
}

void AWeapon_Machine::HandleEndFire()
{
	Super::HandleEndFire();
	GetWorld()->GetTimerManager().ClearTimer(Timer_OnStartFire);
	if (Role == ROLE_Authority)
	{
		MultiSimulateLoop(false);
	}
//	UE_LOG(LogTemp, Warning, TEXT("HandleEndFire"));
}

void AWeapon_Machine::HandleFiring()
{
	Super::HandleFiring();
	if (!CanLoop())
	{
		HandleEndFire();
	}
}

bool AWeapon_Machine::CanLoop()
{
	return PawnOwner && PawnOwner->CanFire()	&& AmmoCount > 0;;
}

void AWeapon_Machine::FireWeapon()
{
	const FVector AimDir = GetAdjustAim();
	const FVector StartLocation = GetMuzzleLocation();
	const float DamageRange = WeaponConfig.WeaponRange;
	const FHitResult HitResult = PawnOwner->TraceForCrossHair(DamageRange, COLLISION_WEAPON);

	ProcessMachineHit(HitResult, StartLocation, AimDir);
}

// void AWeapon_Machine::SimulateWeaponFire()
// {
// 	Super::SimulateWeaponFire();
// 	if (!(GetNetMode() == NM_DedicatedServer || PawnOwner->IsLocallyControlled()))
// 	{
// 		if (BurstCounter!=0)
// 		{
// 			SimulateMachineHit(HitNotify.Origin, HitNotify.Impact, HitNotify.ShooterDir);
// 		}		
// 	}
// }

void AWeapon_Machine::ProcessMachineHit(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir)
{
	if (PawnOwner&& PawnOwner->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		if (Impact.GetActor()/* && Impact.Actor->GetRemoteRole()==ROLE_Authority*/)
		{
			ServerNotifyHit(Impact, ShootDir);
		}
		else if (!Impact.GetActor())
		{
			if (Impact.bBlockingHit)
			{
				ServerNotifyHit(Impact, ShootDir);
			}
			else
			{
				ServerNotifyMiss(Impact, ShootDir);
			}
		}
	}
	ProcessMachineHit_Confirm(Impact, Orign, ShootDir);
}

bool AWeapon_Machine::ServerNotifyHit_Validate(FHitResult Impact, FVector ShootDir)
{
	return true;
}

void AWeapon_Machine::ServerNotifyHit_Implementation(FHitResult Impact, FVector ShootDir)
{
	FVector Origin = GetMuzzleLocation();
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit))
	{
		if (Impact.GetActor() == nullptr && Impact.bBlockingHit)
		{
			ProcessMachineHit_Confirm(Impact, Origin, ShootDir);
		}
		else
		{
			ProcessMachineHit_Confirm(Impact, Origin, ShootDir);
		}
	}
	//	UE_LOG(LogTemp, Warning, TEXT("ServerNotifyHit"));

}
bool AWeapon_Machine::ServerNotifyMiss_Validate(FHitResult Impact, FVector ShootDir)
{
	return true;
}

void AWeapon_Machine::ServerNotifyMiss_Implementation(FHitResult Impact, FVector ShootDir)
{
	//	UE_LOG(LogTemp, Warning, TEXT("ServerNotifyMiss"));
}
bool AWeapon_Machine::MultiSimulateLoop_Validate(bool bsifiring)
{
	return true;
}
void AWeapon_Machine::MultiSimulateLoop_Implementation(bool bsifiring)
{
//	if (Role!=ROLE_AutonomousProxy)
	{
		if (FireAudioCom)
		{
			FireAudioCom->SetActive(bsifiring);
		}
		if (FireParticleCom)
		{
			FireParticleCom->SetActive(bsifiring);
		}
	}
}

void AWeapon_Machine::ProcessMachineHit_Confirm(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir/*, int32 RandomSeed, float ReticleSpreed*/)
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

		//生成击中特效
		SpawnImpactEffects(Impact);
	}
}

void AWeapon_Machine::SimulateMachineHit(const FVector& Origin, const FHitResult& Impact, const FVector& ShooterDir)
{
	if (Impact.bBlockingHit)
	{
		SpawnImpactEffects(Impact);
	}
}

void AWeapon_Machine::SpawnImpactEffects(const FHitResult& Impact)
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

void AWeapon_Machine::OnRep_HitNotify()
{
	//	UE_LOG(LogTemp, Warning, TEXT("OnRep_HitNotify"));
	SimulateMachineHit(HitNotify.Origin, HitNotify.Impact, HitNotify.ShooterDir);

}

void AWeapon_Machine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AWeapon_Machine, HitNotify, COND_SkipOwner);

}