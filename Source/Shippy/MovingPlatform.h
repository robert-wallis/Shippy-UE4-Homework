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

private:
	FVector originalLocation;

	AMovingPlatform(const FObjectInitializer& ObjectInitializer);
	
	void BeginPlay() override;
	
	void Tick(float deltaSeconds) override;

};
