// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class COOPGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:

	/* Time between powerup ticks */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	/* Total times we apply the powerup effect */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TotalNumberOfTicks;

	FTimerHandle TimerHandle_PowerupTick;

	// Total number of ticks applied
	int32 TicksProcessed;

	UFUNCTION()
	void OnTickPowerup();

	// Keeps state of the power-up
	UPROPERTY(ReplicatedUsing = OnRep_PowerupActive)
	bool bIsPowerupActive;

	UFUNCTION()
	void OnRep_PowerupActive();

	// Rotation and vertical movement
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RotationSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float VerticalSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float VerticalRange = 0.25f;

	float VerticalAngle = 0.0f;


public:

	void ActivatePowerup(AActor* ActiveFor);

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupStateChanged(bool nNewIsActive);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};
