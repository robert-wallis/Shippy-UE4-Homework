// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShippyGameMode.h"
#include "ShippyCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "ShippyGameInstance.h"
#include "UObject/ConstructorHelpers.h"

AShippyGameMode::AShippyGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AShippyGameMode::CountDownToTravel()
{
	GetWorld()->GetTimerManager().SetTimer(TravelTimer, this, &AShippyGameMode::TravelToPuzzle, 3.0f, false, 0.f);
}

void AShippyGameMode::TravelToPuzzle()
{
	SetupSeamlessTravel();
	auto ShippyGameInstance = Cast<UShippyGameInstance>(GetGameInstance());
	if (ShippyGameInstance != nullptr) {
		ShippyGameInstance->StartSession();
	}
	GetWorld()->ServerTravel("/Game/Platform/Maps/PuzzleRoom?listen");
}

void AShippyGameMode::TravelToLobby()
{
	SetupSeamlessTravel();
	GetWorld()->ServerTravel("/Game/Platform/Maps/Lobby?listen");
}

void AShippyGameMode::SetupSeamlessTravel()
{
	auto ShippyGameInstance = Cast<UShippyGameInstance>(GetGameInstance());
	if (ShippyGameInstance != nullptr) {
		bUseSeamlessTravel = ShippyGameInstance->IsSeamlessTravelSupported();
	}
}
