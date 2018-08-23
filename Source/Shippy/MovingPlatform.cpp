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

void AMovingPlatform::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);
	if (!HasAuthority())
		return;

	if (MovingTriggerCount > 0) {
		AliveTime += deltaSeconds;
		TickMovePlatform();
	}
}

void AMovingPlatform::TickMovePlatform()
{
	auto t = (sin(AliveTime * Speed) + 1.0f) * 0.5;
	auto location = OriginalLocation + (TargetLocation * t);
	SetActorLocation(location, false);
}