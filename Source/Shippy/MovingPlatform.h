// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

/**
 * Static object that automatically moves.
 */
UCLASS()
class SHIPPY_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Platform, Meta=(MakeEditWidget=true))
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, Category=Platform)
	float Speed = 1.0f;

	UPROPERTY(EditAnywhere, Category=Platform)
	int MovingTriggerCount = 1;

private:
	FVector OriginalLocation;
	float AliveTime = 0;

	AMovingPlatform(const FObjectInitializer& ObjectInitializer);
	
	void BeginPlay() override;
	
	void Tick(float DeltaSeconds) override;

	void TickMovePlatform();

};
