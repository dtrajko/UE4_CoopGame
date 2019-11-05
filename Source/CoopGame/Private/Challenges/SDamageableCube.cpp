// Fill out your copyright notice in the Description page of Project Settings.

#include "SDamageableCube.h"
#include "SHealthComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASDamageableCube::ASDamageableCube()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	// Set to physics body to let radial component affect us (eg. when a nearby barrel explodes)
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASDamageableCube::HandleTakeDamage);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	// BoxComp->SetSphereRadius(100);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Block);
	// BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComp->SetupAttachment(RootComponent);

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->Radius = 0;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false; // Prevent component from ticking, and only use FireImpulse() instead
	RadialForceComp->bIgnoreOwningActor = true; // ignore self

	bExploded = false;
	ExplosionImpulse = 0;
	ExplosionDamage = 60.0f;
	ExplosionRadius = 350;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void ASDamageableCube::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("ASDamageableCube::HandleTakeDamage Health: %s"), *FString::SanitizeFloat(Health));

	ApplyDamage(Health);

	if (Health <= 0.0f)
	{
		SelfDestruct();
	}
}

void ASDamageableCube::ApplyDamage(float Health)
{
	UGameplayStatics::ApplyDamage(this, Health, GetInstigatorController(), this, nullptr);
}

void ASDamageableCube::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	// Explode!
	bExploded = true;

	// Play FX and change self material to black
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	if (ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	}

	MeshComp->SetVisibility(false, true);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Override material on mesh with blackened version
	MeshComp->SetMaterial(0, ExplodedMaterial);

	// Boost the barrel upwards
	FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
	MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

	// Blast away nearby physics actors
	RadialForceComp->FireImpulse();

	if (Role == ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		// Apply damage!
		UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		SetLifeSpan(2.0f);
	}
}
