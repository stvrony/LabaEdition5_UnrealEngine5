// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoTurretComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
UAutoTurretComponent::UAutoTurretComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAutoTurretComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UAutoTurretComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!TurretRotationPart || !MuzzlePoint || !BulletClass)
	{
		return;
	}

	AActor* Player = UGameplayStatics::GetPlayerPawn(this, 0);

	if (!Player)
	{
		return;
	}

	const float Distance = FVector::Dist(GetOwner()->GetActorLocation(), Player->GetActorLocation());
	
	if (Distance > Range)
	{
		return;
	}

	if (!HasLineOfSight(Player))
	{
		return;
	}

	const FVector Direction = Player->GetActorLocation() - TurretRotationPart->GetComponentLocation();
	FRotator TargetRotation = Direction.Rotation();
	TargetRotation.Roll = 0.0f;

	const FRotator NewRotation = FMath::RInterpConstantTo(TurretRotationPart->GetComponentRotation(), TargetRotation, DeltaTime, RotationSpeed);
	TurretRotationPart->SetWorldRotation(NewRotation);

	const float YawDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(NewRotation.Yaw, TargetRotation.Yaw));
	const float PitchDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(NewRotation.Pitch, TargetRotation.Pitch));

	if (YawDiff >= 5.0f || PitchDiff >= 5.0f)
	{
		return;
	}

	const float FireInterval = 1.0f / FMath::Max(FireRate, 0.01f);
	const float Now = GetWorld()->GetTimeSeconds();

	if (Now - LastShotTime >= FireInterval)
	{
		Fire();
	}
}


void UAutoTurretComponent::Initialize(USceneComponent* InTurretRotationPart, USceneComponent* InMuzzlePoint)
{
	TurretRotationPart = InTurretRotationPart;
	MuzzlePoint = InMuzzlePoint;
}


bool UAutoTurretComponent::HasLineOfSight(AActor* Target) const
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	for (TActorIterator<AActor> It(GetWorld(), BulletClass); It; ++It)
	{
		Params.AddIgnoredActor(*It);
	}

	const FVector Start = MuzzlePoint->GetComponentLocation();
	const FVector End = Target->GetActorLocation();

	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	return bHit && Hit.GetActor() == Target;
}


void UAutoTurretComponent::Fire()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	LastShotTime = CurrentTime;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(BulletClass, MuzzlePoint->GetComponentLocation(), MuzzlePoint->GetComponentRotation(), SpawnParams);

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzlePoint->GetComponentLocation(), MuzzlePoint->GetComponentRotation());
	}
}