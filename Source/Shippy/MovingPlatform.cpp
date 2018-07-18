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
	originalLocation = GetActorLocation();
}

void AMovingPlatform::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);
	if (!HasAuthority())
		return;
	auto world = GetWorld();
	auto gameTime = world->RealTimeSeconds;
	auto location = originalLocation + FVector(0.0f, sin(gameTime) * MoveDistance, 0.0f);
	SetActorLocation(location, false);
}
