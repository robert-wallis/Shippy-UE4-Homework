// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "ShippyGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

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
		UE_LOG(LogTemp, Warning, TEXT("MainMenu is _not_ a UMainMenu"));
		return;
	}
	MainMenuWidget->SetInterface(this);

	InGameMenuWidget = CreateWidget<UInGameMenu>(this, InGameMenuClass);
	if (InGameMenuWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameMenu is _not_ a UInGameMenu"));
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
	EnableMouseControl(*playerController);
	FocusOnWidget(*MainMenuWidget, *playerController, FName("HostButton"));
}

void UShippyGameInstance::MainMenuHost()
{
	ClientMessage("Hosting Game");
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
	playerController->ClientTravel(Address, ::TRAVEL_Relative);

	if (MainMenuWidget != nullptr) {
		MainMenuWidget->RemoveFromViewport();
	}
}

void UShippyGameInstance::InGameMenu()
{
	if (InGameMenuClass == nullptr)
		return;

	InGameMenuWidget->AddToViewport();

	const auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	EnableMouseControl(*playerController);
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
}

void UShippyGameInstance::EnableMouseControl(APlayerController& playerController)
{
	playerController.bShowMouseCursor = true;
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
