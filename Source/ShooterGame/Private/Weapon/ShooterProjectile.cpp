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

	//��ײ����
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	//��ײ��ѯ��������ģ��
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	RootComponent = CollisionComp;

	//�ӵ��ƶ����
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	ProjectileMovementComp->UpdatedComponent = CollisionComp;
	ProjectileMovementComp->InitialSpeed = 2000.f;
	ProjectileMovementComp->MaxSpeed = 2000.f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->ProjectileGravityScale = 0.f;
	ProjectileMovementComp->bInitialVelocityInLocalSpace = false;

	//����
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

	//�ӵ�ֹͣʱ ����Ӧ����
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->OnProjectileStop.AddDynamic(this, &AShooterProjectile::OnImpact);
	}
	//�ƶ������ ���ú���Instigator	//������ʱ��Instigator���ϲ㣨���������룬ΪCharactor
	if (CollisionComp)
	{
		CollisionComp->MoveIgnoreActors.Add(Instigator);
	}
	//��ȡ�ӵ���Ϣ
	AWeapon_Projectile* WeaponPro = Cast<AWeapon_Projectile>(GetOwner());
	if (WeaponPro)
	{
		WeaponPro->ApplyWeaponConfig(WeaponConfig);
	}

	MyController=GetInstigatorController();
}

//��ըʵ��
void AShooterProjectile::OnImpact(const FHitResult& ImpactResult)
{

	Explode(ImpactResult);
	Destroy();
}

void AShooterProjectile::Explode(const FHitResult& ImpackResult)
{
	//ȡ���ӵ� ��Ч
	if (ParticleSystemCom)
	{
		ParticleSystemCom->Deactivate();
	}

	//�����˺�
	const FVector ImpackLocation = ImpackResult.ImpactPoint + ImpackResult.ImpactNormal*10.f;
	if (WeaponConfig.ExplosionDamage>0.f && WeaponConfig.ExplosionRadius>0.f)
	{

		UGameplayStatics::ApplyRadialDamage(this, WeaponConfig.ExplosionDamage, ImpackLocation, WeaponConfig.ExplosionRadius, WeaponConfig.DamageType, TArray<AActor*>(), this, MyController.Get()/*��ȡ��ָ�����*/);
	}

	//���ɱ�ը����
	if (ExplosionEffectClass)
	{
		const FTransform SpawnTransform(ImpackResult.Normal.Rotation(), ImpackLocation);
		//��ʱ ����Actor
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