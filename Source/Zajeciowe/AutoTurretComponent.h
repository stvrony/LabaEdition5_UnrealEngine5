// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AutoTurretComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZAJECIOWE_API UAutoTurretComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAutoTurretComponent();

	UFUNCTION(BlueprintCallable, Category = "Turret")
	void Initialize(USceneComponent* InTurretRotationPart, USceneComponent* InMuzzlePoint);



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	UPROPERTY(EditAnywhere, Category = "Turret", BlueprintReadWrite)
	TSubclassOf<AActor> BulletClass;

	UPROPERTY(EditAnywhere, Category = "Turret", BlueprintReadWrite)
	float Range = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Turret", BlueprintReadWrite)
	float FireRate = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Turret", BlueprintReadWrite)
	float RotationSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Turret", BlueprintReadWrite)
	float Damage = 5.0f;

private:
	UPROPERTY()
	USceneComponent* TurretRotationPart = nullptr;

	UPROPERTY()
	USceneComponent* MuzzlePoint = nullptr;

	float TimeSinceLastShot = 0.0f;
	bool HasLineOfSight(AActor* Target) const;
	void Fire();
};
