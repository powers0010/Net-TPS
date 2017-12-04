// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon_Sniper.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "ShooterCharacter.h"
#include "ShooterGame.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ShooterImpactEffect.h"


AWeapon_Sniper::AWeapon_Sniper() :TargetFOV(20.f), NormalFov(90.f)
{
	SniperViewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SniperView"));
	SniperViewMesh->SetupAttachment(RootComponent);
	SniperViewMesh->CastShadow = false;
	StoreCameraSpringArm = 0.f;
}

void AWeapon_Sniper::AttachWeaponToPawn()
{
	Super::AttachWeaponToPawn();
	if (SniperViewMesh)
	{
		SniperViewMesh->SetHiddenInGame(false);
	}
}

void AWeapon_Sniper::DetachWeaponFromPawn()
{
	Super::DetachWeaponFromPawn();
	if (SniperViewMesh)
	{
		SniperViewMesh->SetHiddenInGame(true);
	}
}

void AWeapon_Sniper::FireWeapon()
{
	const FVector AimDir = GetAdjustAim();
	const FVector StartLocation = GetMuzzleLocation();
	const float DamageRange = WeaponConfig.WeaponRange;
	const FHitResult HitResult = PawnOwner->TraceForCrossHair(DamageRange, COLLISION_WEAPON);

	ProcessSniperHit(HitResult, StartLocation, AimDir);
}

void AWeapon_Sniper::OnStartTarget()
{
	if (PawnOwner)
	{
		StoreCameraSpringArm = PawnOwner->GetCurrentCameraSpringArm();
		PawnOwner->SetCameraSpringArm(0.f);
		PawnOwner->SetSnperTargetWidgetVisbility(true);
		PawnOwner->SetFOV(TargetFOV);
	}
}

void AWeapon_Sniper::OnStopTarget()
{
	if (PawnOwner)
	{
		if (StoreCameraSpringArm!=0)
		{
			PawnOwner->SetCameraSpringArm(StoreCameraSpringArm);
		}
		
		PawnOwner->SetSnperTargetWidgetVisbility(false);
		PawnOwner->SetFOV(NormalFov);
//		UE_LOG(LogTemp, Warning, TEXT("OnStopTarget"));
	}
}
bool AWeapon_Sniper::CanFire() const
{
	bool Localb = Super::CanFire();
	return Localb&&PawnOwner->GetIsTargeting();
}

void AWeapon_Sniper::SpawnImpactEffects(const FHitResult& Impact)
{
	if (ImpactEffectActor && Impact.bBlockingHit)
	{
		const FTransform SpawnTrans(Impact.Normal.Rotation(), Impact.ImpactPoint);
		AShooterImpactEffect* ImpactEffect = GetWorld()->SpawnActorDeferred<AShooterImpactEffect>(ImpactEffectActor, SpawnTrans);
		if (ImpactEffect)
		{
			UGameplayStatics::FinishSpawningActor(ImpactEffect, SpawnTrans);
		}
	}
}

void AWeapon_Sniper::ProcessSniperHit(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir)
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
	ProcessSniprHit_Confirm(Impact, Orign, ShootDir);
}

void AWeapon_Sniper::ProcessSniprHit_Confirm(const FHitResult& Impact, const FVector& Orign, const FVector& ShootDir)
{
	//生命值处理
	if (Impact.GetActor() && ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}

	if (Role == ROLE_Authority)
	{
//		HitNotify.HitNum = HitNotify.HitNum++%20;
		HitNotify.Origin = Orign;
		HitNotify.Impact = Impact;
		HitNotify.ShooterDir = ShootDir;
	}

	if (GetNetMode() != NM_DedicatedServer/*PawnOwner&& PawnOwner->IsLocallyControlled()*/)
	{
		//生成开枪特效
		const FVector EndTrace = Orign + ShootDir*WeaponConfig.WeaponRange;
		const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
		//生成击中特效
		SpawnImpactEffects(Impact);
	}
}
// 
// void AWeapon_Sniper::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
// {
// 	
// }

void AWeapon_Sniper::OnRep_HitNotify()
{
	//	UE_LOG(LogTemp, Warning, TEXT("OnRep_HitNotify"));
	SpawnImpactEffects(HitNotify.Impact);
}

void AWeapon_Sniper::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AWeapon_Sniper, HitNotify, COND_SkipOwner);

}

bool AWeapon_Sniper::ServerNotifyHit_Validate(FHitResult Impact, FVector ShootDir)
{
	return true;
}

void AWeapon_Sniper::ServerNotifyHit_Implementation(FHitResult Impact, FVector ShootDir)
{
	FVector Origin = GetMuzzleLocation();
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit))
	{
		if (Impact.GetActor() == nullptr && Impact.bBlockingHit)
		{
			ProcessSniprHit_Confirm(Impact, Origin, ShootDir);
		}
		else
		{
			ProcessSniprHit_Confirm(Impact, Origin, ShootDir);
		}
	}
	//	UE_LOG(LogTemp, Warning, TEXT("ServerNotifyHit"));
}

bool AWeapon_Sniper::ServerNotifyMiss_Validate(FHitResult Impact, FVector ShootDir)
{
	return true;
}

void AWeapon_Sniper::ServerNotifyMiss_Implementation(FHitResult Impact, FVector ShootDir)
{
	//	UE_LOG(LogTemp, Warning, TEXT("ServerNotifyMiss"));
}