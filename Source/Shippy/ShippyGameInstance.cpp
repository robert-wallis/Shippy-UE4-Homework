// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "ShippyGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

#include "MainMenuWidget.h"
#include "InGameMenu.h"

UShippyGameInstance::UShippyGameInstance(const FObjectInitializer &ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/Menu/WBP_MainMenu"));
	MainMenuClass = MainMenuBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/Menu/InGameMenu_WBP"));
	InGameMenuClass = InGameMenuBPClass.Class;
}

void UShippyGameInstance::Init()
{
}

void UShippyGameInstance::MainMenu()
{
	if (MainMenuClass == nullptr)
		return;

	auto menuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuClass);
	if (menuWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenu is _not_ a UMainMenuWidget"));
		return;
	}
	menuWidget->AddToViewport();
	menuWidget->SetMenuInterface(this);

	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	EnableMouseControl(*playerController);
	FocusOnWidget(*menuWidget, *playerController, FName("HostButton"));
}

void UShippyGameInstance::MainMenuHost()
{
	GetEngine()->AddOnScreenDebugMessage(0, 3.0f, FColor::White, TEXT("Hosting"));
	GetWorld()->ServerTravel("/Game/Platform/Maps/PuzzleRoom?listen");
}

void UShippyGameInstance::MainMenuJoinGame(const FString& Address)
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController != nullptr) {
		auto message = FString::Printf(TEXT("Joining %s"), *Address);
		GetEngine()->AddOnScreenDebugMessage(0, 3.0f, FColor::White, *message);
		playerController->ClientTravel(Address, ::TRAVEL_Relative);
	}
}

void UShippyGameInstance::InGameMenu()
{
	if (InGameMenuClass == nullptr)
		return;

	auto menuWidget = CreateWidget<UInGameMenu>(this, InGameMenuClass);
	if (menuWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameMenu is _not_ a UInGameMenu"));
		return;
	}
	menuWidget->AddToViewport();
	menuWidget->SetInterface(this);
	CurrentInGameMenu = menuWidget;

	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	EnableMouseControl(*playerController);
	FocusOnWidget(*menuWidget, *playerController, FName("CancelButton"));
}

void UShippyGameInstance::InGameMenuExitToMainMenu()
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	playerController->ClientTravel("/Game/Menu/MainMenu", ::TRAVEL_Relative);
}

void UShippyGameInstance::InGameMenuCancel()
{
	if (CurrentInGameMenu == nullptr)
		return;
	CurrentInGameMenu->RemoveFromViewport();
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
