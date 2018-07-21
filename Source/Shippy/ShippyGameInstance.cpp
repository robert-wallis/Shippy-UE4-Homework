// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "ShippyGameInstance.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

void UShippyGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("shippy init"));
}

void UShippyGameInstance::Host()
{
	UE_LOG(LogTemp, Warning, TEXT("Host Called"));
	GetWorld()->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
}

void UShippyGameInstance::Join(const FString& Address)
{
	auto pc = GetFirstLocalPlayerController();
	if (pc != nullptr) {
		auto message = FString::Printf(TEXT("Join %s"), *Address);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *message);
		GetEngine()->AddOnScreenDebugMessage(0, 3.0f, FColor::Blue, *message);
		pc->ClientTravel(Address, ::TRAVEL_Absolute);
	}
}
