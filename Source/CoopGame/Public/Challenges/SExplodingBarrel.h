// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SExplodingBarrel.generated.h"


class USHealthComponent;
class UMaterialInstanceDynamic;
class UBoxComponent;
class USoundCue;
class URadialForceComponent;


UCLASS()
class COOPGAME_API ASExplodingBarrel : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASExplodingBarrel();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		UBoxComponent* BoxComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		URadialForceComponent* RadialForceComp;

	UFUNCTION()
		void HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
			class AController* InstigatedBy, AActor* DamageCauser);

	// Dynamic material to pulse on damage
	UMaterialInstanceDynamic* MatInst;

	bool bExploded;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category = "ExplodingBarrel")
		UParticleSystem* ExplosionEffect;

	/* Impulse applied to the barrel mesh when it explodes to boost it up a little */
	UPROPERTY(EditDefaultsOnly, Category = "ExplodingBarrel")
		float ExplosionImpulse;

	UPROPERTY(EditDefaultsOnly, Category = "ExplodingBarrel")
		float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "ExplodingBarrel")
		float ExplosionDamage;

	void DamageSelf();

	UPROPERTY(EditDefaultsOnly, Category = "ExplodingBarrel")
		USoundCue* ExplodeSound;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
