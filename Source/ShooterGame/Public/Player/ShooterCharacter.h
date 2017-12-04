// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"


class AWeapon;
class USpringArmComponent;

UENUM()
enum class EPerspective : uint8
{
	EPerspective_MIN,
	EPerspective_FPP,
	EPerspective_TPP,
	EPerspective_MAX
};


UCLASS()
class SHOOTERGAME_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//更新相机位置
	void OnCameraUpdata(const FVector& CameraLocation,const FRotator& CameraRotation);

	//初始化组件 调用
	virtual void PostInitializeComponents();

	//伤害接口
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate  = 1.f , FName StartSectionName  = NAME_None ) override;

	virtual void StopAnimMontage(class UAnimMontage* AnimMontage  = NULL ) override;
	
	FHitResult TraceForCrossHair(float TraceRange, ECollisionChannel TaceChannel) const;

	void SetSnperTargetWidgetVisbility(bool bIsVisible);

	void SetFOV(float _FOV);

	void SetCameraSpringArm(float newarm);

	float GetCurrentCameraSpringArm();
	//控制输入
	void MoveForward(float value);
	void MoveRight(float value);
	void OnStartTarget();
	void OnEndTarget();
	void OnStartFire();
	void OnStopFire();
	void OnReload();
	void OnEquip();

	UFUNCTION(BlueprintCallable, Category = Zoom)
		void ZoomIn(float value);
	UFUNCTION(BlueprintCallable, Category = Zoom)
		void ZoomOut(float value);
	//获取 变量的接口
	USkeletalMeshComponent* GetPawnMesh();

	USkeletalMeshComponent* GetMesh1P();

	USkeletalMeshComponent* GetMesh3P();

	FName GetWeaponAttachPoint() const;

	UFUNCTION(BlueprintCallable , Category = Weapon)
	FRotator GetAimOffsets() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
		bool GetIsTargeting() const;
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void SetIsTargeting(bool NewIsTarget);

	UFUNCTION(BlueprintPure, Category = Health)
		int32 GetCurHealth() const;
	UFUNCTION(BlueprintPure, Category = Health)
		int32 GetMaxHealth() const;

	AWeapon* GetCurrentWeapon();

	EPerspective GetPerspectiveType();

	bool CanFire();

	bool IsAlive();
protected:

	/*UPROPERTY(EditAnywhere, category = "Camera")*/
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera1P;

	UPROPERTY(Category = "Character", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArm;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera3P;

	/** The main skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditDefaultsOnly , Category = Weapon )
		TSubclassOf<AWeapon> WeaponClass;
// 
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
 		AWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<AWeapon>> DefaultInventorySystem;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
		TArray<AWeapon*> Inventory;

	void SpawnDefaultInventory();

	void AddWeapon(AWeapon* NewWeapon);

	void EquipWeapon(AWeapon* Weapon);

	void SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon);

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
		FName WeaponAttachPoint;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
		bool bIsTargeting;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = Health)
		float CurHealth;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, EditDefaultsOnly ,Category = Health)
		float MaxHealth;

	UPROPERTY(Transient, BlueprintReadOnly, Category = ViewType)
		EPerspective PerspectiveType;

	UPROPERTY(Transient, BlueprintReadWrite, Category = ViewType)
		UUserWidget* SniperTargetWidget;
	//------replicated-------------
	UFUNCTION()
		void OnRep_CurrentWeapon(AWeapon* LastWeapon);

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerEquipWeapon(AWeapon* Weapon);

public:
	UPROPERTY(Transient, Replicated)
		bool bIsEquiping;
	UPROPERTY(Transient, Replicated)
		bool bIsReloading;

//------------------------视角切换-------------

public:
	UFUNCTION(BlueprintCallable, Category = ViewType)
	void ChangeToFPP();

	UFUNCTION(BlueprintCallable, Category = ViewType)
	void ChangeToTPP();
};
