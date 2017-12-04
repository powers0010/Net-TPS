// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "ShooterGame.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ExplosionEffect.h"
#include "Net/UnrealNetwork.h"



// Sets default values
AWeapon::AWeapon() : AmmoCount(50)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	RootComponent = Mesh1P;
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh1P->SetCollisionResponseToChannels(ECR_Ignore);
	Mesh1P->CastShadow = false;
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->SetRelativeTransform(FTransform(FRotator(0.f, 0.f, -90.f)));

	FireSound = nullptr;
	PawnOwner = nullptr;

	PreWeaponState = WeaponState::Idle;
	CurWeaponState = WeaponState::Idle;
// 	bIsEquipped = false;
// 	bWantToReload = false;
// 	bWantToFiring = false;
// 	bWantToEquip = false;
// 	bReFiring = false;
	bPlayingFiringAnim = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	ClipCount = WeaponConfig.ClipAmount;
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, BurstCounter);
	DOREPLIFETIME_CONDITION(AWeapon, AmmoCount, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeapon, ClipCount, COND_OwnerOnly);

}

void AWeapon::AttachWeaponToPawn()
{
	if (PawnOwner)
	{
		USkeletalMeshComponent* PawnMesh1P = PawnOwner->GetMesh3P();
		FName WeaponPoint = WeaponAttachPoint;// PawnOwner->GetWeaponAttachPoint();
		if (PawnMesh1P)
		{
			Mesh1P->SetHiddenInGame(false);
			Mesh1P->AttachToComponent(PawnMesh1P, FAttachmentTransformRules::KeepRelativeTransform, WeaponPoint);
		}
	}
}

void AWeapon::DetachWeaponFromPawn()
{
//	Mesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh1P->SetHiddenInGame(true);
}


void AWeapon::SetPawnOwner(AShooterCharacter* Pawn)
{
	if (Pawn)
	{
		PawnOwner = Pawn;
	}
}

void AWeapon::OnStartTarget()
{

}

void AWeapon::OnStopTarget()
{

}

void AWeapon::OnEquip(const AWeapon* _LastWeapon)
{
	LastWeapon = (AWeapon*)_LastWeapon;
	if (LastWeapon)
	{
		LastWeapon->OnStopTarget();
	}
	HandleStartEquip();
// 	if (!bWantToEquip)
// 	{
// 		bWantToEquip = true;
// 		CaculateEquipState();
// 		DependOnCurrentWeaponState();
// 	}

}

void AWeapon::OnUnEquip()
{
	//卸载mesh
	DetachWeaponFromPawn();
//	bIsEquipped = false;
	StopFire();

// 	if (bWantToReload)
// 	{
// 		bWantToReload = false;
// 
// 		//停止播放装弹动画
// 		StopMontageAnimation(ReloadAnim);
// 
// 		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
// 		GetWorldTimerManager().ClearTimer( TimerHandle_ReloadWeapon);
// 	}
// 
// 	if (bWantToEquip)
// 	{
// 		bWantToEquip = false;
// 		//停止播放 装备动画
// 		StopMontageAnimation(EquipAnim);
// 
// 		GetWorldTimerManager().ClearTimer( TimerHandle_OnEquipFinish);
// 	}

//	DependOnCurrentWeaponState();
}

FVector AWeapon::GetAdjustAim()
{
	FVector AimDir = FVector::ZeroVector;
	AShooterPlayerController* PlayerController = Instigator ? Cast<AShooterPlayerController>(Instigator->GetController()) : nullptr;
	if (PlayerController)
	{
		FVector Location;
		FRotator Rotation;
		//获取 发射方向
		PlayerController->GetPlayerViewPoint(Location, Rotation);
		AimDir = Rotation.Vector();
	}
//	UE_LOG(LogTemp, Warning, TEXT("AimDir: %s"), *AimDir.ToString());
	return AimDir;
}

FHitResult AWeapon::WeaponTrace(FVector TraceFrom, FVector TraceTo) const
{
	static FName weapontag = FName(TEXT("Weapontag"));
	FCollisionQueryParams TraceParams(weapontag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult HitResult(ForceInit);
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceFrom, TraceTo, COLLISION_WEAPON, TraceParams);

	return HitResult;
}

FVector AWeapon::GetMuzzleLocation()
{
	if (Mesh1P)
	{
		return Mesh1P->GetSocketLocation(MuzzleAttachPoint);
	}
	return FVector::ZeroVector;
}

FRotator AWeapon::GetMuzzleRotation()
{
	if (Mesh1P)
	{
		return Mesh1P->GetSocketRotation(MuzzleAttachPoint);
	}
	return FRotator::ZeroRotator;
}

void AWeapon::StartFire()
{
	if (Role<ROLE_Authority)
	{
		ServerStartFire();
	}
	HandleStartFire();
}

void AWeapon::StopFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}
	HandleEndFire();
}

void AWeapon::HandleFiring()
{
	if ( CanFire())
	{
		if (GetNetMode()!= NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}
		if (PawnOwner && PawnOwner->IsLocallyControlled())
		{
			UseAmmo();
			FireWeapon();
			BurstCounter++;
		}
		if (Role == ROLE_Authority)
		{
			UseAmmo();
			//to simulate in remote client
			BurstCounter++;
		}
		LastFireTime = GetWorld()->GetTimeSeconds();

		if (PawnOwner && PawnOwner->IsLocallyControlled())
		{
			if (Role < ROLE_Authority)
			{
				ServerStartFire();
			}
		}
	}
}


void AWeapon::SimulateWeaponFire()
{
	if (Role == ROLE_Authority && CanFire() == false)
	{
		return;
	}
// 	if (PawnOwner && PawnOwner->IsLocallyControlled())
// 	{
		if (FireEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(FireEffect, RootComponent, MuzzleAttachPoint);
		}
/*	}*/

	if (FireSound)
	{
		PlayWeaponSound(FireSound);
	}
	//开火动画
	if (!bPlayingFiringAnim)
	{
		bPlayingFiringAnim = true;
		PlayMontageAnimation(FireAnim);
	}

	APlayerController* PC = PawnOwner ? Cast<APlayerController>(PawnOwner->GetController()) : nullptr;
	if (PC && PC->IsLocalController())
	{
		if (FireCameraShake)
		{
			AShooterPlayerController* PC = PawnOwner != nullptr ? Cast<AShooterPlayerController>(PawnOwner->Controller) : nullptr;
			if (PC && PC->IsLocalController())
			{
				PC->ClientPlayCameraShake(FireCameraShake);
			}
		}
	}
}

void AWeapon::UseAmmo()
{
	if (AmmoCount>0)
	{
		AmmoCount--;
	}
}

void AWeapon::StopSimulateWeaponFire()
{
	//todo
	if (bPlayingFiringAnim)
	{
		bPlayingFiringAnim = false;
		StopMontageAnimation(FireAnim);
	}
}

void AWeapon::FireWeapon()
{

}
UAudioComponent* AWeapon::PlayWeaponSound(USoundCue* FireSound)
{
	UAudioComponent* AC = nullptr;
	if (FireSound)
	{
		AC = UGameplayStatics::SpawnSoundAttached(FireSound, PawnOwner->GetRootComponent());
	}
	return AC;
}

int32 AWeapon::GetCurrentAmmoAmount() const
{
	return AmmoCount;
}

int32 AWeapon::GetMaxAmmoAmount() const
{
	return GetClass()->GetDefaultObject<AWeapon>()->AmmoCount;
}

int32 AWeapon::GetClipAmount() const
{
	return ClipCount;
}

// void AWeapon::CaculateEquipState()
// {
// 	WeaponState::Type NewState = WeaponState::Idle;
// 	if (bIsEquipped)
// 	{
// 		if (bWantToReload /*&& CanRealod()*/)
// 		{
// 			if (CanRealod()==false)
// 			{
// 				NewState = CurWeaponState;
// 			}
// 			else
// 			{
// 				NewState = WeaponState::Reloading;
// 			}
// 		}
// 		else if((bWantToReload == false)&&(bWantToFiring==true))
// 		{
// 			NewState = WeaponState::Firing;
// 		}
// 	}
// 	else 
// 	{
// 		if (bWantToEquip)
// 		{
// 			NewState = WeaponState::Equiping;
// 		}	
// 	}
// //	UE_LOG(LogTemp, Warning, TEXT(" Caculate WeaponState"));
// 	
// 	SetWeaponState(NewState);
// }

// void AWeapon::DependOnCurrentWeaponState()
// {
// 	if (PreWeaponState == WeaponState::Idle && CurWeaponState == WeaponState::Firing)
// 	{
// 		//处理开始开火
// 		HandleStartFire();
// 	}
// 	else if(PreWeaponState == WeaponState::Firing && (CurWeaponState == WeaponState::Idle || CurWeaponState == WeaponState::Reloading))
// 	{
// 		//处理结束开火
// 		HandleEndFire();
// 	}

// 	if ((PreWeaponState == WeaponState::Idle || PreWeaponState == WeaponState::Firing) && CurWeaponState == WeaponState::Reloading)
// 	{
// 		//开始装弹
// 		HandleStartReload();
// 	} 
// 	else if(PreWeaponState == WeaponState::Reloading && CurWeaponState == WeaponState::Idle)
// 	{
// 		//结束装弹
// 		HandleEndReload();
// 	}
// 
// 	else if (PreWeaponState == WeaponState::Idle && CurWeaponState == WeaponState::Equiping)
// 	{
// 		//开始更换装备
// 		HandleStartEquip();
// 	} 
// 	else if(PreWeaponState == WeaponState::Equiping && CurWeaponState == WeaponState::Idle)
// 	{
// 		//结束更换装备
// 		HandleEndEquip();
// 	}
//}

void AWeapon::SetWeaponState(WeaponState::Type NewWeaponState)
{
	PreWeaponState = CurWeaponState;
	CurWeaponState = NewWeaponState;
}

bool AWeapon::CanFire() const
{
	bool LocalCanFire = PawnOwner && PawnOwner->CanFire() 
		&&(GetWorld()->GetTimeSeconds()-LastFireTime>WeaponConfig.FireCD)
		&&AmmoCount>0;
	return LocalCanFire;
}

bool AWeapon::CanRealod()
{
	return (GetCurrentAmmoAmount() < GetMaxAmmoAmount())&&(ClipCount > 0);
}

void AWeapon::HandleStartFire()
{
	HandleFiring();
}

void AWeapon::HandleEndFire()
{
	if (PawnOwner)
	{
		PawnOwner->OnEndTarget();
	}
	BurstCounter = 0;
	StopSimulateWeaponFire();
//	bReFiring = false;
//	GetWorldTimerManager().ClearTimer(TimerHandle_HandleReFiring);
}

void AWeapon::HandleStartReload()
{
	PawnOwner->bIsReloading = true;
	HandleEndFire();

	float AnimDurTime = PlayMontageAnimation(ReloadAnim);

	if (AnimDurTime<=0.f)
	{
		AnimDurTime = 0.5f;
	}
	//PlayAnim

	GetWorldTimerManager().SetTimer( TimerHandle_StopReload, this, &AWeapon::StopReload, AnimDurTime, false);
	
	if (Role==ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &AWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDurTime - 0.1f), false);
	}

	if (PawnOwner && PawnOwner->IsLocallyControlled())
	{
		PlayWeaponSound(ReloadSound);
	}
}

void AWeapon::HandleEndReload()
{
	PawnOwner->bIsReloading = false;
	//停止动画
	StopMontageAnimation(ReloadAnim);

	GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
	GetWorldTimerManager().ClearTimer( TimerHandle_ReloadWeapon);

	AmmoCount = FMath::Min(AmmoCount+WeaponConfig.AmmoPerClip ,GetMaxAmmoAmount());
	ClipCount--;
//	UE_LOG(LogTemp, Warning, TEXT("ClipCount %d"), ClipCount);
}

void AWeapon::HandleStartEquip()
{
	PawnOwner->bIsEquiping = true;
	HandleEndFire();
	AttachWeaponToPawn();
// 	if (Role==ROLE_Authority)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("ROLE_Authority HandleStartEquip"));
// 	}
// 	else if (Role == ROLE_AutonomousProxy)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("ROLE_AutonomousProxy HandleStartEquip"));
// 	}

	if (LastWeapon)	//换枪
	{
		LastWeapon->OnUnEquip();
		float AnimDurTime = PlayMontageAnimation(EquipAnim);
		//PlayAnim
		if (AnimDurTime<=0.f)
		{
			AnimDurTime = 1.f;
		}
		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinish, this, &AWeapon::OnEquipFinish, AnimDurTime, false);
	}
	else			//角色生成时进行
	{
		OnEquipFinish();
	}
	
	if (PawnOwner)
	{
		PlayWeaponSound(EquipSound);
	}
//	bIsEquipped = true;
}

void AWeapon::HandleEndEquip()
{
	PawnOwner->bIsEquiping = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinish);

	//停止武器动画
	StopMontageAnimation(EquipAnim);
}

void AWeapon::OnEquipFinish()
{
// 	if (!bIsEquipped)
// 	{
// 		bIsEquipped = true;
// 	}
// 	if (bWantToEquip)
// 	{
// 		bWantToEquip = false;
// 	}
// 	CaculateEquipState();
// 	DependOnCurrentWeaponState();
	HandleEndEquip();
}

void AWeapon::StartReload()
{
	if (Role < ROLE_Authority)
	{
		ServerStartReload();
	}
	if (/*!bWantToReload */CanRealod())
	{
// 		bWantToReload = true;
// 		CaculateEquipState();
// 		DependOnCurrentWeaponState();
//		UE_LOG(LogTemp, Warning, TEXT("StartReload"));
		HandleStartReload();
	}
	else
	{
		PawnOwner->bIsReloading = false;
	}
}

void AWeapon::StopReload()
{
// 	if (bWantToReload)
// 	{
// 		bWantToReload = false;
// 		CaculateEquipState();
// 		DependOnCurrentWeaponState();
// 	}

	HandleEndReload();
}

void AWeapon::ReloadWeapon()
{

}

WeaponState::Type AWeapon::GetCurrentWeaponState()
{
	return CurWeaponState;
}

float AWeapon::PlayMontageAnimation(const FWeaponAnim Animation)
{
	float during = 0.f;
	if (PawnOwner)
	{
		UAnimMontage* UseMontage = /*PawnOwner->GetPerspectiveType() < EPerspective::EPerspective_TPP ? Animation.Pawn1P : */Animation.Pawn3P;

		if (UseMontage)
		{
			during = PawnOwner->PlayAnimMontage(UseMontage);
// 			if (Role == ROLE_Authority)
// 			{
// 				UE_LOG(LogTemp, Warning, TEXT("ROLE_Authority PlayAnimMontage"));
// 			}
		}
	}
	return during;
}

void AWeapon::StopMontageAnimation(const FWeaponAnim Animation)
{
	if (PawnOwner)
	{
		UAnimMontage* UseMontage = Animation.Pawn3P;
		if (UseMontage)
		{
			PawnOwner->StopAnimMontage(UseMontage);
		}
	}
}

bool AWeapon::ServerStartFire_Validate()
{
	return true;
}

void AWeapon::ServerStartFire_Implementation()
{
	StartFire();
}
bool AWeapon::ServerStopFire_Validate()
{
	return true;
}

void AWeapon::ServerStopFire_Implementation()
{
	StopFire();
//	UE_LOG(LogTemp, Warning, TEXT("ServerStopFire_Implementation"));
}

bool AWeapon::ServerStartReload_Validate()
{
	return true;
}

void AWeapon::ServerStartReload_Implementation()
{
	StartReload();
}

void AWeapon::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopSimulateWeaponFire();
	}
//	UE_LOG(LogTemp, Warning, TEXT("OnRep_BurstCounter"));
}


bool AWeapon::ShouldDealDamage(AActor* TestActor) const
{
	// if we're an actor on the server, or the actor's role is authoritative, we should register damage
	if (TestActor)
	{
		if (GetNetMode() != NM_Client ||
			TestActor->Role == ROLE_Authority ||
			TestActor->bTearOff)
		{
			return true;
		}
	}

	return false;
}


void AWeapon::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	FPointDamageEvent PointDamage;
	PointDamage.DamageTypeClass = WeaponConfig.DamageType;
	PointDamage.HitInfo = Impact;
	PointDamage.ShotDirection = ShootDir;
	PointDamage.Damage = WeaponConfig.damage;

	if (Impact.GetActor())
	{
		Impact.GetActor()->TakeDamage(PointDamage.Damage, PointDamage, PawnOwner->GetController(), this);
	}
}