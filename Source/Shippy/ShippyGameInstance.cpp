// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "ShippyGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "PlatformTrigger.h"
#include "MainMenuWidget.h"

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
}

void UShippyGameInstance::MainMenu()
{
	if (MainMenuClass == nullptr)
		return;

	auto menuWidget = CreateWidget<UUserWidget>(this, MainMenuClass);
	if (menuWidget == nullptr)
		return;
	menuWidget->AddToViewport();

	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;

	playerController->bShowMouseCursor = true;

	auto joinAddressWidget = menuWidget->GetWidgetFromName(FName("JoinAddressEdit"));
	if (joinAddressWidget == nullptr)
		return;

	UE_LOG(LogTemp, Warning, TEXT("setting focus to join address"));
	joinAddressWidget->SetUserFocus(playerController);
	joinAddressWidget->SetKeyboardFocus();
}

void UShippyGameInstance::Host()
{
	GetEngine()->AddOnScreenDebugMessage(0, 3.0f, FColor::Blue, TEXT("Hosting"));
	GetWorld()->ServerTravel("/Game/Platform/Maps/PuzzleRoom?listen");
}

void UShippyGameInstance::Join(const FString& Address)
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController != nullptr) {
		auto message = FString::Printf(TEXT("Join %s"), *Address);
		GetEngine()->AddOnScreenDebugMessage(0, 3.0f, FColor::Blue, *message);
		playerController->ClientTravel(Address, ::TRAVEL_Absolute);
	}
}
