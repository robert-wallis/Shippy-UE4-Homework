// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "ShippyGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "LogShippy.h"

UShippyGameInstance::UShippyGameInstance(const FObjectInitializer &ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/Menu/MainMenu_WBP"));
	MainMenuClass = MainMenuBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/Menu/InGameMenu_WBP"));
	InGameMenuClass = InGameMenuBPClass.Class;

}

void UShippyGameInstance::Init()
{
	MainMenuWidget = CreateWidget<UMainMenu>(this, MainMenuClass);
	if (MainMenuWidget == nullptr)
	{
		UE_LOG(LogShippy, Warning, TEXT("MainMenu is _not_ a UMainMenu"));
		return;
	}
	MainMenuWidget->SetInterface(this);

	InGameMenuWidget = CreateWidget<UInGameMenu>(this, InGameMenuClass);
	if (InGameMenuWidget == nullptr)
	{
		UE_LOG(LogShippy, Warning, TEXT("InGameMenu is _not_ a UInGameMenu"));
		return;
	}
	InGameMenuWidget->SetInterface(this);
}

void UShippyGameInstance::MainMenu()
{
	if (MainMenuClass == nullptr)
		return;

	MainMenuWidget->AddToViewport();

	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	MouseInputForMenu(*playerController);
	FocusOnWidget(*MainMenuWidget, *playerController, FName("HostButton"));
}

void UShippyGameInstance::MainMenuHost()
{
	ClientMessage("Hosting Game");
	MouseInputForGame();
	GetWorld()->ServerTravel("/Game/Platform/Maps/PuzzleRoom?listen");
}

void UShippyGameInstance::MainMenuJoinGame(const FString& Address)
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr) {
		return;
	}

	const auto message = FString::Printf(TEXT("Joining %s"), *Address);
	ClientMessage(message);

	if (MainMenuWidget != nullptr) {
		MainMenuWidget->RemoveFromViewport();
		MouseInputForGame();
	}

	playerController->ClientTravel(Address, ::TRAVEL_Relative);
}

void UShippyGameInstance::MainMenuQuit()
{
	ClientMessage("MainMenu Quit");
	GetFirstGamePlayer()->ConsoleCommand("QUIT");
}

void UShippyGameInstance::InGameMenu()
{
	if (InGameMenuClass == nullptr)
		return;

	InGameMenuWidget->AddToViewport();

	const auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	MouseInputForMenu(*playerController);
	FocusOnWidget(*InGameMenuWidget, *playerController, FName("CancelButton"));
}

void UShippyGameInstance::InGameMenuExitToMainMenu()
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	InGameMenuWidget->RemoveFromViewport();
	ClientMessage("Left Game");
	playerController->ClientTravel("/Game/Menu/MainMenu", ::TRAVEL_Relative);
}

void UShippyGameInstance::InGameMenuCancel()
{
	if (InGameMenuWidget == nullptr)
		return;
	InGameMenuWidget->RemoveFromViewport();
	MouseInputForGame();
}

void UShippyGameInstance::MouseInputForMenu(APlayerController& playerController)
{
	playerController.bShowMouseCursor = true;

	FInputModeUIOnly inputMode;
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	playerController.SetInputMode(inputMode);
}

void UShippyGameInstance::MouseInputForGame()
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	playerController->bShowMouseCursor = false;

	FInputModeGameOnly inputMode;
	playerController->SetInputMode(inputMode);
}

void UShippyGameInstance::FocusOnWidget(UUserWidget& menuWidget, APlayerController& playerController, const FName& name)
{
	auto defaultWidget = menuWidget.GetWidgetFromName(name);
	if (defaultWidget == nullptr)
		return;

	defaultWidget->SetUserFocus(&playerController);
	defaultWidget->SetKeyboardFocus();
}

void UShippyGameInstance::ClientMessage(const FString & message)
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController != nullptr)
	{
		playerController->ClientMessage(message);
	}
}
