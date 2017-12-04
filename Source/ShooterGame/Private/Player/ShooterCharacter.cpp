// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterCharacter.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "ShooterGame.h"
#include "Weapon.h"
#include "ShooterPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


// Sets default values
AShooterCharacter::AShooterCharacter():MaxHealth(1000)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Camera1P
// 	Camera1P = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera1P"));
// 	Camera1P->SetupAttachment(RootComponent);
// 	Camera1P->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight));

	//Camera3P
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	Camera3P = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera3P"));
	Camera3P->SetupAttachment(SpringArm);
	
	//Mesh碰撞	//第三人称模型
	//自己不可见 //不接受 贴花投射
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	//碰撞设置
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PICKUP, ECR_Ignore);

	//设置胶囊体碰撞通道
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PICKUP, ECR_Ignore);
	
	//第一人称 模型
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	/*	Mesh1P->SetupAttachment(Camera1P);*/
	Mesh1P->SetupAttachment(GetCapsuleComponent());
	//仅自己可见 //不接受 贴花投射	//不投影
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->bCastDynamicShadow = false;
	//骨骼动画的pose更新 只发生在渲染时
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	//Mesh1P 更新组 设置为 在物理之前更新的更新组
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	//关闭 第一人称碰撞
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToChannels(ECR_Ignore);
	//	Mesh1P->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	SniperTargetWidget = nullptr;
	bIsTargeting = false;
	bReplicates = true;

	bIsEquiping = false;
	bIsReloading = false;
	PerspectiveType = EPerspective::EPerspective_FPP;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurHealth = MaxHealth;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// OwnerOnly
	DOREPLIFETIME_CONDITION(AShooterCharacter, Inventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, bIsEquiping, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, bIsReloading, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, MaxHealth, COND_OwnerOnly);

	// everyone
	DOREPLIFETIME(AShooterCharacter, CurrentWeapon);
	DOREPLIFETIME(AShooterCharacter, CurHealth);
}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Role == ROLE_Authority)
	{
		SpawnDefaultInventory();
	}
}

void AShooterCharacter::OnCameraUpdata(const FVector& CameraLocation, const FRotator& CameraRotation)
{
	
	USkeletalMeshComponent* DefaultMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("Mesh1P")));
	if (!DefaultMesh1P)
	{
		return;
	}

	//获取默认Mesh1P组件相对于Root的变换矩阵 
	const FMatrix DefaultMesh1PLS = FRotationTranslationMatrix(DefaultMesh1P->RelativeRotation, DefaultMesh1P->RelativeLocation);

	//本地空间到世界空间的转化矩阵
	const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

	//相机在世界坐标的旋转矩阵 Yaw 和Pitch
	const FRotator CameraYaw(0.f, CameraRotation.Yaw, 0.f);
	const FRotator CameraYawAndPitch(CameraRotation.Pitch, CameraRotation.Yaw, 0.f);

	//camera相对于root的变换 本地空间
	const FMatrix CameraToRootInLS = FRotationTranslationMatrix(CameraYawAndPitch, CameraLocation)*LocalToWorld.Inverse();

	//camera相对于root在Yaw的变换 本地空间
	const FMatrix CameraToRootYawInLS = FRotationTranslationMatrix(CameraYaw, CameraLocation)*LocalToWorld.Inverse();

	//计算Mesh1P的相对坐标
	const FMatrix Mesh1PRelative = DefaultMesh1PLS * CameraToRootYawInLS.Inverse() * CameraToRootInLS ;

	//设置坐标
	Mesh1P->SetRelativeLocationAndRotation(Mesh1PRelative.GetOrigin(), Mesh1PRelative.Rotator());
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AShooterCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AShooterCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Target"), IE_Pressed, this, &AShooterCharacter::OnStartTarget);
	PlayerInputComponent->BindAction(TEXT("Target"), IE_Released, this, &AShooterCharacter::OnEndTarget);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AShooterCharacter::OnStartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AShooterCharacter::OnStopFire);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AShooterCharacter::OnReload);
	PlayerInputComponent->BindAction(TEXT("Equip"), IE_Pressed, this, &AShooterCharacter::OnEquip);

}

void AShooterCharacter::MoveForward(float value)
{
	if (Controller)
	{
		bool bLimitRotation = GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling();
		FRotator Rotator = bLimitRotation ? GetActorRotation() : GetControlRotation();
		FVector Direction = FRotationMatrix(Rotator).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}
void AShooterCharacter::MoveRight(float value)
{
	if (Controller)
	{
		//		bool bLimitRotation = GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling();
		//		FRotator Rotator = bLimitRotation ? GetActorRotation() : GetControlRotation();
		FRotator Rotator = GetActorRotation();
		FVector Direction = FRotationMatrix(Rotator).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, value);
	}
}
void AShooterCharacter::ZoomIn(float value)
{
	if (Controller && SpringArm && PerspectiveType != EPerspective::EPerspective_FPP)
	{
		SpringArm->TargetArmLength = FMath::Clamp<float>(SpringArm->TargetArmLength - 10.f, 50.f, 500.f);
	}
}
void AShooterCharacter::ZoomOut(float value)
{
	if (Controller && SpringArm && PerspectiveType!= EPerspective::EPerspective_FPP)
	{
		SpringArm->TargetArmLength = FMath::Clamp<float>(SpringArm->TargetArmLength + 10.f, 50.f, 500.f);
	}
}

void AShooterCharacter::OnStartTarget()
{
	AShooterPlayerController* MyController = Cast<AShooterPlayerController>(Controller);
	if (MyController)
	{
		SetIsTargeting(true);
		if (CurrentWeapon && IsLocallyControlled())
		{
			CurrentWeapon->OnStartTarget();
		}
	}
//	UE_LOG(LogTemp, Warning, TEXT("OnStartTarget"));
}
void AShooterCharacter::OnEndTarget()
{
	SetIsTargeting(false);
	if (CurrentWeapon && IsLocallyControlled())
	{
		CurrentWeapon->OnStopTarget();
	}
}

void AShooterCharacter::OnStartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}
void AShooterCharacter::OnStopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void AShooterCharacter::OnReload()
{
	if (bIsReloading || bIsEquiping)
	{
		return;
	}
	AShooterPlayerController* MyController = Cast<AShooterPlayerController>(GetController());
	if (MyController)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->StartReload();
//			UE_LOG(LogTemp, Warning, TEXT("OnReload"));
		}
	}
}

void AShooterCharacter::OnEquip()
{
	if (bIsReloading || bIsEquiping)
	{
		return;
	}
	AShooterPlayerController* MyController = Cast<AShooterPlayerController>(GetController());
	if (MyController)
	{
		if (Inventory.Num()>0 && CurrentWeapon && CurrentWeapon->GetCurrentWeaponState() != WeaponState::Equiping)
		{
			const int32 CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);
			const int32 NextWeaponIndex = (CurrentWeaponIndex + 1) % Inventory.Num();
			AWeapon* NextWeapon = Inventory[NextWeaponIndex];

			EquipWeapon(NextWeapon);
		}
	}
}

USkeletalMeshComponent* AShooterCharacter::GetMesh1P()
{
	return Mesh1P;
}
USkeletalMeshComponent* AShooterCharacter::GetMesh3P()
{
	return GetMesh();
}

FName AShooterCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

FRotator AShooterCharacter::GetAimOffsets() const
{
	//获取Aim方向 //世界坐标系
	FVector AimDirWS = GetBaseAimRotation().Vector();
	//获取Local To World 的变换Transform，用 矩阵的逆矩阵 变换 一个向量
	//把Aim方向从世界坐标系变换到Local坐标系
	FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	return AimDirLS.Rotation();
//	FRotator AimOffset = FRotator::ZeroRotator;
// 	if (CurrentWeapon)
// 	{
// 		const FRotator CameraRotation = Camera3P->GetComponentRotation();
// 		const FVector MuzzleLocation = CurrentWeapon->GetMuzzleLocation();
// 		const FVector CameraHit = TraceForCrossHair(1000.f, COLLISION_WEAPON).ImpactPoint;
// 
// 		const FRotator MuzzleLocationToCameraHit = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, CameraHit);
// 		const FRotator CameraRot_Muzzle = UKismetMathLibrary::NormalizedDeltaRotator(CameraRotation, MuzzleLocationToCameraHit);
// 
// 		const FRotator CameraInLocal = Camera3P->GetRelativeTransform().Rotator();
// 
// 		AimOffset.Pitch = -1.f *(UKismetMathLibrary::NormalizedDeltaRotator(CameraInLocal, MuzzleLocationToCameraHit)).Pitch;
// 		AimOffset.Yaw = /*-1.f **/ (UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), MuzzleLocationToCameraHit)).Yaw;
// 		
// 	}
// 	return AimOffset;
}

bool AShooterCharacter::GetIsTargeting() const
{
	return bIsTargeting;
}

void AShooterCharacter::SetIsTargeting(bool NewIsTarget)
{
	bIsTargeting = NewIsTarget;
}

void AShooterCharacter::SetSnperTargetWidgetVisbility(bool bIsVisible)
{
	if (SniperTargetWidget)
	{
		SniperTargetWidget->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

float AShooterCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//保存 经过父类计算的实际伤害
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage>0.f)
	{
 		CurHealth -= ActualDamage;
		UE_LOG(LogTemp, Warning, TEXT("Health: %f"), CurHealth);
 	}

	return ActualDamage;
}

FHitResult AShooterCharacter::TraceForCrossHair(float TraceRange, ECollisionChannel TaceChannel) const
{
	FHitResult HitResult(ForceInit);
	if (Camera3P)
	{
		const FTransform CameraTransform = Camera3P->GetComponentTransform();
		const FVector CameraLocation = CameraTransform.GetLocation();
		const FVector TargetLocation = CameraTransform.GetLocation() + TraceRange*CameraTransform.GetRotation().Vector();

		//Trace
		static FName weapontag = FName(TEXT("Weapontag"));
		FCollisionQueryParams TraceParams(weapontag, true, Instigator);
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = true;
//		GetWorld()->DebugDrawTraceTag = weapontag;
		GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, TargetLocation, TaceChannel, TraceParams);		
	}
	return HitResult;
}


float AShooterCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate , FName StartSectionName)
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (UseMesh && AnimMontage && UseMesh->AnimScriptInstance)
	{
		return UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
	}
	return 0.f;
}

void AShooterCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (UseMesh && AnimMontage && UseMesh->AnimScriptInstance)
	{
		UseMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime(), AnimMontage);
	}
}

USkeletalMeshComponent* AShooterCharacter::GetPawnMesh()
{
	return /*PerspectiveType < EPerspective::EPerspective_TPP ? Mesh1P : */GetMesh();
}

int32 AShooterCharacter::GetCurHealth() const
{
	return CurHealth;
}

int32 AShooterCharacter::GetMaxHealth() const
{
	return MaxHealth;//GetClass()->GetDefaultObject<AShooterCharacter>()->Health;
}

AWeapon* AShooterCharacter::GetCurrentWeapon()
{
	return CurrentWeapon ? CurrentWeapon : nullptr;
}

EPerspective AShooterCharacter::GetPerspectiveType()
{
	return PerspectiveType;
}

bool AShooterCharacter::CanFire()
{
	return IsAlive() && !bIsEquiping && !bIsReloading;
}

bool AShooterCharacter::IsAlive()
{
	return CurHealth > 0;
}

void AShooterCharacter::SpawnDefaultInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}
	int32 NumOfDefaultInventory = DefaultInventorySystem.Num();
	for (int32 i = 0;i< NumOfDefaultInventory; i++)
	{
		if (DefaultInventorySystem[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultInventorySystem[i], SpawnInfo);
			if (NewWeapon)
			{
				AddWeapon(NewWeapon);
			}
		}
	}

	if (Inventory.Num()>0&& Inventory[0])
	{
		CurrentWeapon = Inventory[0];
		CurrentWeapon->OnEquip(nullptr);
	}
}

void AShooterCharacter::AddWeapon(AWeapon* NewWeapon)
{
	if (NewWeapon && Role == ROLE_Authority)
	{
		NewWeapon->SetPawnOwner(this);
		NewWeapon->SetOwner(this);
		NewWeapon->Instigator = Instigator;
		Inventory.AddUnique(NewWeapon);
	}
}

void AShooterCharacter::EquipWeapon(AWeapon* Weapon)
{
	if (Weapon)
	{
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

void AShooterCharacter::SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon)
{

	bIsEquiping = true;

	AWeapon* LocalWeapon = nullptr;
	if (LastWeapon)
	{
		LocalWeapon = LastWeapon;
	}
	else if (NewWeapon!= CurrentWeapon)
	{
		LocalWeapon = CurrentWeapon;
	}

// 	if (LocalWeapon)
// 	{
// 		LocalWeapon->OnUnEquip();
// 	}
	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		NewWeapon->SetPawnOwner(this);
		NewWeapon->Instigator = Instigator;
		NewWeapon->OnEquip(LastWeapon);
	}
	else
	{
		bIsEquiping = false;
	}

}

void AShooterCharacter::OnRep_CurrentWeapon(AWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
//	UE_LOG(LogTemp, Warning, TEXT("OnRep_CurrentWeapon"));
}

void AShooterCharacter::ServerEquipWeapon_Implementation(AWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

bool AShooterCharacter::ServerEquipWeapon_Validate(AWeapon* Weapon)
{
	return true;
}

void AShooterCharacter::ChangeToFPP()
{
	APlayerController* MyController = Cast<APlayerController>(Controller);
	if (MyController&& SpringArm&& Camera3P/* && Camera1P*/)
	{
		SpringArm->TargetArmLength = 0.f;
		PerspectiveType = EPerspective::EPerspective_FPP;
	}
}
void AShooterCharacter::ChangeToTPP()
{
	APlayerController* MyController = Cast<APlayerController>(Controller);
	if (MyController&& Camera3P/* && Camera1P*/)
	{
		SpringArm->TargetArmLength = 300.f;
		PerspectiveType = EPerspective::EPerspective_TPP;
	}
}

void AShooterCharacter::SetFOV(float _FOV)
{
	AShooterPlayerController* PC = Cast<AShooterPlayerController>(Controller);
	if (PC && PC->IsLocalController())
	{
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
		if (CameraManager)
		{
			CameraManager->SetFOV(_FOV);
		}	
	}
}

void AShooterCharacter::SetCameraSpringArm(float newarm)
{
	if (SpringArm)
	{
		SpringArm->TargetArmLength = newarm;
	}
}

float AShooterCharacter::GetCurrentCameraSpringArm()
{
	return SpringArm ? SpringArm->TargetArmLength : 0.f;
}