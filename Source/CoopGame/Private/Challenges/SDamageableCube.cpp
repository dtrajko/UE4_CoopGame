// Fill out your copyright notice in the Description page of Project Settings.

#include "SDamageableCube.h"
#include "SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASDamageableCube::ASDamageableCube()
{
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASDamageableCube::OnHealthChanged);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	// Set to physics body to let radial component affect us (eg. when a nearby barrel explodes)
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->Radius = 0;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false; // Prevent component from ticking, and only use FireImpulse() instead
	RadialForceComp->bIgnoreOwningActor = true; // ignore self

	ExplosionImpulse = 0;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void ASDamageableCube::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bExploded)
	{
		// Nothing left to do, already exploded.
		return;
	}

	if (Health <= 0.0f)
	{
		// Explode!
		bExploded = true;
		OnRep_Exploded();

		// Boost the barrel upwards
		FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

		// Blast away nearby physics actors
		RadialForceComp->FireImpulse();

		if (ExplosionSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
		}

		Destroy();
	}
}

void ASDamageableCube::OnRep_Exploded()
{
	// Play FX and change self material to black
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	// Override material on mesh with blackened version
	MeshComp->SetMaterial(0, ExplodedMaterial);

	Destroy();
}

void ASDamageableCube::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDamageableCube, bExploded);
}
