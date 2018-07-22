// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "ShippyGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "PlatformTrigger.h"
#include "Blueprint/UserWidget.h"

UShippyGameInstance::UShippyGameInstance(const FObjectInitializer &ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/Menu/WBP_MainMenu"));
	if (MainMenuBPClass.Class != NULL)
	{
		MainMenuClass = MainMenuBPClass.Class;
	}
}

void UShippyGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("shippy init"));
}

void UShippyGameInstance::MainMenu()
{
	if (MainMenuClass == nullptr)
		return;

	auto menuWidget = CreateWidget<UUserWidget>(this, MainMenuClass);
	menuWidget->AddToViewport();

	auto pc = GetFirstLocalPlayerController();
	if (pc != nullptr)
	{
		pc->bShowMouseCursor = true;
	}
}

void UShippyGameInstance::Host()
{
	GetWorld()->ServerTravel("/Game/Platform/Maps/PuzzleRoom?listen");
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
