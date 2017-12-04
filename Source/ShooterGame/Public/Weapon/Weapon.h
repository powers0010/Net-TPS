// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class AShooterCharacter;
class UAudioComponent;
class USoundCue;
class UParticleSystem;
class UAnimMontage;

namespace WeaponState
{
	UENUM()
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equiping
	};
}

USTRUCT()
struct FImpactHitInfo
{
	GENERATED_USTRUCT_BODY()
// 	UPROPERTY()
// 		int HitNum;
	UPROPERTY()
		FVector Origin;
	UPROPERTY()
		FHitResult Impact;
	UPROPERTY()
		FVector ShooterDir;
};

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, category = WeaponData)
	int32 AmmoPerClip;	//��������

	UPROPERTY(EditDefaultsOnly, category = WeaponData)
	int32 ClipAmount;	//��������

	UPROPERTY(EditDefaultsOnly , category = WeaponData)
	float FireCD;		//������

	UPROPERTY(EditDefaultsOnly, category = WeaponData)
		float WeaponRange;

	UPROPERTY(EditDefaultsOnly, category = WeaponData)
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, category = WeaponData)
		int32 damage;

	FWeaponData()
	{
		FireCD = 1.f;
		AmmoPerClip = 10;
		ClipAmount = 5;
	}
};

USTRUCT()
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, category = Animation)
	UAnimMontage* Pawn1P;

	UPROPERTY(EditDefaultsOnly, category = Animation)
		UAnimMontage* Pawn3P;
};

UCLASS()
class SHOOTERGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void AttachWeaponToPawn();

	virtual void DetachWeaponFromPawn();

	void SetPawnOwner(AShooterCharacter* Pawn);

	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool ShouldDealDamage(AActor* TestActor) const;

	virtual void DealDamage(const FHitResult& Impact, const FVector& ShootDir);
protected:
	AShooterCharacter* PawnOwner;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh1P;

	//�����ӵ���SocketName
	UPROPERTY(EditDefaultsOnly , Category = Weapon)
	FName MuzzleAttachPoint;
	
	//��ǹ��Ч
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireSound;

	//��ǹ��Ч
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* EquipSound;

	//��ǹ��Ч
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* ReloadSound;

	//��ǹ��Ч
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	UParticleSystem* FireEffect;

	//�ӵ�����
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, EditDefaultsOnly, Category = Weapon)
	int32 AmmoCount;

	//��������
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, EditDefaultsOnly, Category = Weapon)
	int32 ClipCount;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FWeaponData WeaponConfig;

	bool bPlayingFiringAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim FireAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim EquipAnim;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim ReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		TSubclassOf<UCameraShake> FireCameraShake;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
		int32 BurstCounter;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
		FName WeaponAttachPoint;

	float LastFireTime;

	//-----------������״̬�л�״̬���ء�-------------------
// 	bool bIsEquipped;
// 	bool bWantToReload;
// 	bool bWantToFiring;
// 	bool bWantToEquip;
// 	bool bReFiring;



	//-------
	virtual bool CanFire() const;

	bool CanRealod();
	WeaponState::Type CurWeaponState;
	WeaponState::Type PreWeaponState;

	AWeapon* LastWeapon;

	FTimerHandle TimerHandle_OnEquipFinish;
	FTimerHandle TimerHandle_ReloadWeapon;
	FTimerHandle TimerHandle_StopReload;
	FTimerHandle TimerHandle_HandleReFiring;

	void OnEquipFinish();

public:	
	//��ȡ��׼���� //�ӵ����䷽�� //����������
	FVector GetAdjustAim();

	FHitResult WeaponTrace(FVector TraceFrom, FVector TraceTo) const;

	//----------������--------------
	void StartFire();

	void StopFire();

	//����ʵ�ֿ���״̬
	virtual void HandleFiring();

	virtual void SimulateWeaponFire();

	virtual void StopSimulateWeaponFire();

	virtual void FireWeapon();

	virtual void OnStartTarget();

	virtual void OnStopTarget();

	UAudioComponent* PlayWeaponSound(USoundCue* FireSound);

	FVector GetMuzzleLocation();

	FRotator GetMuzzleRotation();

	void OnEquip(const AWeapon* _LastWeapon);

	void OnUnEquip();

	//----------������״̬��
	void CaculateEquipState();
	
	void SetWeaponState(WeaponState::Type NewWeaponState);

	void DependOnCurrentWeaponState();

	virtual void HandleStartFire();

	virtual void HandleEndFire();

	void HandleStartReload();

	void HandleEndReload();

	void HandleStartEquip();

	void HandleEndEquip();
	//��ʼװ���ӵ�
	void StartReload();
	//����װ���ӵ�
	void StopReload();
	//�����ӵ�����
	void ReloadWeapon();

	float PlayMontageAnimation(const FWeaponAnim Animation);

	void StopMontageAnimation(const FWeaponAnim Animation);

	void UseAmmo();

	//��ȡ�ӵ�����
	int32 GetCurrentAmmoAmount() const;
	int32 GetMaxAmmoAmount() const;
	int32 GetClipAmount() const;

	WeaponState::Type GetCurrentWeaponState();

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerStartFire();

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerStopFire();

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerStartReload();

	UFUNCTION()
		void OnRep_BurstCounter();
};
