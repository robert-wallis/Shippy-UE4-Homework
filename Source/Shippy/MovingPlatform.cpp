// Fill out your copyright notice in the Description page of Project Settings.

#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	OriginalLocation = GetActorLocation();
	if (HasAuthority()) {
		SetReplicates(true);
		SetReplicateMovement(true);
		TickMovePlatform();
	}
}

void AMovingPlatform::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!HasAuthority())
		return;

	if (MovingTriggerCount > 0) {
		AliveTime += DeltaSeconds;
		TickMovePlatform();
	}
}

void AMovingPlatform::TickMovePlatform()
{
	auto Time = (sin(AliveTime * Speed) + 1.0f) * 0.5;
	auto Location = OriginalLocation + (TargetLocation * Time);
	SetActorLocation(Location, false);
}