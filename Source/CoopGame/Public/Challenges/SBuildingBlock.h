// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SBuildingBlock.generated.h"


class USHealthComponent;
class UMaterialInstanceDynamic;
class UBoxComponent;
class USoundCue;


UCLASS()
class COOPGAME_API ASBuildingBlock : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASBuildingBlock();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		UBoxComponent* BoxComp;

	UFUNCTION()
		void HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
			class AController* InstigatedBy, AActor* DamageCauser);

	// Dynamic material to pulse on damage
	UMaterialInstanceDynamic* MatInst;

	bool bExploded;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category = "BuildingBlock")
		UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "BuildingBlock")
		float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "BuildingBlock")
		float ExplosionDamage;

	void DamageSelf();

	UPROPERTY(EditDefaultsOnly, Category = "BuildingBlock")
		USoundCue* ExplodeSound;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
