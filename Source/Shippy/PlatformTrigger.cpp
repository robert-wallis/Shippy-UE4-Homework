// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "PlatformTrigger.h"
#include "MovingPlatform.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
APlatformTrigger::APlatformTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggeringVolume = CreateDefaultSubobject<UBoxComponent>(FName("TriggeringVolume"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMesh"));

	RootComponent = StaticMesh;
	TriggeringVolume->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		TriggeringVolume->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnOverlapBegin);
		TriggeringVolume->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnOverlapEnd);
	}
}

// Called every frame
void APlatformTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlatformTrigger::ActivateTrigger()
{
	for (auto& platform : Platforms) {
		platform->MovingTriggerCount ++;
	}
}

void APlatformTrigger::DeactivateTrigger()
{
	for (auto& platform : Platforms) {
		platform->MovingTriggerCount --;
	}
}

void APlatformTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;

	if (TriggerCount == 0) { // someone triggered it for the first time
		ActivateTrigger();
	}
	TriggerCount ++;
}

void APlatformTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority())
		return;

	TriggerCount --;
	if (TriggerCount == 0) { // the last person has stepped out of the trigger
		DeactivateTrigger();
	}
}

