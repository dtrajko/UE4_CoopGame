// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplodingBarrel.h"
#include "SCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SHealthComponent.h"
#include "Components/BoxComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Character.h"
#include "Kismet\GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Sound/SoundCue.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
ASExplodingBarrel::ASExplodingBarrel()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplodingBarrel::HandleTakeDamage);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetBoxExtent(FVector(38.0f, 38.0f, 50.0f));
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComp->SetupAttachment(RootComponent);

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->SetIsReplicated(true);
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false; // Prevent component from ticking, and only use FireImpulse() instead
	RadialForceComp->bIgnoreOwningActor = true; // ignore self

	ExplosionImpulse = 400;

	bExploded = false;
	ExplosionDamage = 60.0f;
	ExplosionRadius = 350;
}

// Called when the game starts or when spawned
void ASExplodingBarrel::BeginPlay()
{
	Super::BeginPlay();
}

void ASExplodingBarrel::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	// Explode on hitpoints = 0
	if (Health <= 0.0f)
	{
		SelfDestruct();
	}
}

void ASExplodingBarrel::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	if (ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	}

	// Boost the barrel upwards
	FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
	MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

	// Blast away nearby physics actors
	RadialForceComp->FireImpulse();

	MeshComp->SetVisibility(false, true);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Role == ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		// Apply damage!
		UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		SetLifeSpan(2.0f);
	}
}

void ASExplodingBarrel::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ASExplodingBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
