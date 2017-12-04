// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterProjectile.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "ShooterGame.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "ExplosionEffect.h"
#include "Net/UnrealNetwork.h"

/*#include "Weapon/Weapon_Projectile.h"*/

// Sets default values
AShooterProjectile::AShooterProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//碰撞设置
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	//碰撞查询，不进行模拟
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	RootComponent = CollisionComp;

	//子弹移动组件
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	ProjectileMovementComp->UpdatedComponent = CollisionComp;
	ProjectileMovementComp->InitialSpeed = 2000.f;
	ProjectileMovementComp->MaxSpeed = 2000.f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->ProjectileGravityScale = 0.f;
	ProjectileMovementComp->bInitialVelocityInLocalSpace = false;

	//粒子
	ParticleSystemCom = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemCom"));
	ParticleSystemCom->bAutoActivate = true;
	ParticleSystemCom->bAutoDestroy = false;
	ParticleSystemCom->SetupAttachment(RootComponent);

	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateMovement = true;

	bExploded = false;

}

// Called when the game starts or when spawned
void AShooterProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
// void AShooterProjectile::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// 
// }

void AShooterProjectile::InitializeVelocity(FVector ShooterDirection)
{
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->Velocity = ShooterDirection * ProjectileMovementComp->InitialSpeed;
	}
}

void AShooterProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//子弹停止时 绑定响应函数
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->OnProjectileStop.AddDynamic(this, &AShooterProjectile::OnImpact);
	}
	//移动组件中 设置忽略Instigator	//在生成时，Instigator由上层（武器）传入，为Charactor
	if (CollisionComp)
	{
		CollisionComp->MoveIgnoreActors.Add(Instigator);
	}
	//获取子弹信息
	AWeapon_Projectile* WeaponPro = Cast<AWeapon_Projectile>(GetOwner());
	if (WeaponPro)
	{
		WeaponPro->ApplyWeaponConfig(WeaponConfig);
	}

	MyController=GetInstigatorController();
}

//爆炸实现
void AShooterProjectile::OnImpact(const FHitResult& ImpactResult)
{

	Explode(ImpactResult);
	Destroy();
}

void AShooterProjectile::Explode(const FHitResult& ImpackResult)
{
	//取消子弹 特效
	if (ParticleSystemCom)
	{
		ParticleSystemCom->Deactivate();
	}

	//产生伤害
	const FVector ImpackLocation = ImpackResult.ImpactPoint + ImpackResult.ImpactNormal*10.f;
	if (WeaponConfig.ExplosionDamage>0.f && WeaponConfig.ExplosionRadius>0.f)
	{

		UGameplayStatics::ApplyRadialDamage(this, WeaponConfig.ExplosionDamage, ImpackLocation, WeaponConfig.ExplosionRadius, WeaponConfig.DamageType, TArray<AActor*>(), this, MyController.Get()/*获取弱指针对象*/);
	}

	//生成爆炸对象
	if (ExplosionEffectClass)
	{
		const FTransform SpawnTransform(ImpackResult.Normal.Rotation(), ImpackLocation);
		//延时 生成Actor
		AExplosionEffect* ExplosionActor = GetWorld()->SpawnActorDeferred<AExplosionEffect>(ExplosionEffectClass, SpawnTransform);
		UGameplayStatics::FinishSpawningActor(ExplosionActor, SpawnTransform);
	}

	bExploded = true;
}

void AShooterProjectile::OnRep_Exploded()
{
	const FVector location = GetActorLocation();
	const FVector direction = GetActorForwardVector();
	const FVector TraceStart = location - direction * 200;
	const FVector TraceEnd = location + direction * 200;

	FHitResult Impact;
	if (!GetWorld()->LineTraceSingleByChannel(Impact, TraceStart, TraceEnd, COLLISION_PROJECTILE))
	{
		Impact.ImpactPoint = location;
		Impact.ImpactNormal = -direction;
	}
	Explode(Impact);
//	UE_LOG(LogTemp, Warning, TEXT("OnRep_Exploded"));
}

void AShooterProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	Super::PostNetReceiveVelocity(NewVelocity);
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->Velocity = NewVelocity;
	}
}

void AShooterProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterProjectile, bExploded);
}