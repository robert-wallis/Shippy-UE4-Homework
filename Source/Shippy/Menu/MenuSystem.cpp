// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "MenuSystem.h"

#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

#include "MainMenu.h"
#include "InGameMenu.h"

DEFINE_LOG_CATEGORY(LogShippyMenu);

UMenuSystem::UMenuSystem(const FObjectInitializer &ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/Menu/MainMenu_WBP"));
	MainMenuClass = MainMenuBPClass.Class;
	if (MainMenuClass == nullptr) {
		UE_LOG(LogShippyMenu, Error, TEXT("Couldn't find MainMenu_WBP for MainMenuClass"));
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/Menu/InGameMenu_WBP"));
	InGameMenuClass = InGameMenuBPClass.Class;
	if (InGameMenuClass == nullptr) {
		UE_LOG(LogShippyMenu, Error, TEXT("Couldn't find InGameMenu_WBP for InGameMenuClass"));
	}

}

void UMenuSystem::Init(UGameInstance* gameInstance, class MainMenuInterface* mainMenuInterface, class InGameMenuInterface* inGameMenuInterface)
{
	MainMenuWidget = CreateWidget<UMainMenu>(gameInstance, MainMenuClass);
	if (MainMenuWidget == nullptr) {
		UE_LOG(LogShippyMenu, Error, TEXT("MainMenu is _not_ a UMainMenu"));
		return;
	}
	MainMenuWidget->SetInterface(mainMenuInterface);

	InGameMenuWidget = CreateWidget<UInGameMenu>(gameInstance, InGameMenuClass);
	if (InGameMenuWidget == nullptr) {
		UE_LOG(LogShippyMenu, Error, TEXT("InGameMenu is _not_ a UInGameMenu"));
		return;
	}
	InGameMenuWidget->SetInterface(inGameMenuInterface);
}

void UMenuSystem::MainMenuOpen(APlayerController &playerController)
{
	UE_LOG(LogShippyMenu, Log, TEXT("MainMenuOpen"));

	MainMenuWidget->AddToViewport();
	MainMenuWidget->SetVisibility(ESlateVisibility::Visible);

	MouseInputForMenu(playerController);
	FocusOnWidget(playerController, *MainMenuWidget, FName("HostButton"));
}

void UMenuSystem::MainMenuClose(APlayerController &playerController)
{
	if (MainMenuWidget == nullptr)
		return;
	UE_LOG(LogShippyMenu, Log, TEXT("MainMenuClose"));

	MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	MouseInputForGame(playerController);
}

void UMenuSystem::InGameMenuOpen(APlayerController &playerController)
{
	UE_LOG(LogShippyMenu, Log, TEXT("InGameMenuOpen"));

	InGameMenuWidget->AddToViewport();
	InGameMenuWidget->SetVisibility(ESlateVisibility::Visible);

	MouseInputForMenu(playerController);
	FocusOnWidget(playerController, *InGameMenuWidget, FName("CancelButton"));
}

void UMenuSystem::InGameMenuClose(APlayerController &playerController)
{
	if (InGameMenuWidget == nullptr)
		return;
	UE_LOG(LogShippyMenu, Log, TEXT("InGameMenuClose"));

	InGameMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	MouseInputForGame(playerController);
}

void UMenuSystem::SearchClearResults()
{
	if (MainMenuWidget == nullptr)
		return;
	SearchClearResults();
}

void UMenuSystem::SearchAddServer(const FString& name, const FString& address)
{
	if (MainMenuWidget == nullptr)
		return;
	MainMenuWidget->SearchAddServer(name, address);
}

void UMenuSystem::MouseInputForMenu(APlayerController &playerController)
{
	playerController.bShowMouseCursor = true;

	FInputModeUIOnly inputMode;
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	playerController.SetInputMode(inputMode);
}

void UMenuSystem::MouseInputForGame(APlayerController &playerController)
{
	playerController.bShowMouseCursor = false;

	FInputModeGameOnly inputMode;
	playerController.SetInputMode(inputMode);
}

void UMenuSystem::FocusOnWidget(APlayerController &playerController, UUserWidget& menuWidget, const FName& name)
{
	auto defaultWidget = menuWidget.GetWidgetFromName(name);
	if (defaultWidget == nullptr) {
		UE_LOG(LogShippyMenu, Warning, TEXT("UMenuSystem::FocusOnWidget - Widget %s was not found."), *name.ToString())
		return;
	}

	defaultWidget->SetUserFocus(&playerController);
	defaultWidget->SetKeyboardFocus();
}
