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
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/Menu/MainMenu_WBP"));
	MainMenuClass = MainMenuBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/Menu/InGameMenu_WBP"));
	InGameMenuClass = InGameMenuBPClass.Class;

}

void UShippyGameInstance::Init()
{
	MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuClass);
	if (MainMenuWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenu is _not_ a UMainMenuWidget"));
		return;
	}
	MainMenuWidget->SetMenuInterface(this);

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

	if (GEngine == nullptr)
		return;

	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	EnableMouseControl(*playerController);
	FocusOnWidget(*MainMenuWidget, *playerController, FName("HostButton"));
}

void UShippyGameInstance::MainMenuHost()
{
	GetEngine()->AddOnScreenDebugMessage(0, 3.0f, FColor::White, TEXT("Hosting"));
	GetWorld()->ServerTravel("/Game/Platform/Maps/PuzzleRoom?listen");
}

void UShippyGameInstance::MainMenuJoinGame(const FString& Address)
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr) {
		return;
	}

	auto message = FString::Printf(TEXT("Joining %s"), *Address);
	GetEngine()->AddOnScreenDebugMessage(0, 3.0f, FColor::White, *message);
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

	if (GEngine == nullptr)
		return;

	auto playerController = GetFirstLocalPlayerController();
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
