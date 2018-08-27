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

void UMenuSystem::Init(UGameInstance* GameInstance, class MainMenuInterface* MainMenuInterface, class InGameMenuInterface* InGameMenuInterface)
{
	MainMenuWidget = CreateWidget<UMainMenu>(GameInstance, MainMenuClass);
	if (MainMenuWidget == nullptr) {
		UE_LOG(LogShippyMenu, Error, TEXT("MainMenu is _not_ a UMainMenu"));
		return;
	}
	MainMenuWidget->SetInterface(MainMenuInterface);

	InGameMenuWidget = CreateWidget<UInGameMenu>(GameInstance, InGameMenuClass);
	if (InGameMenuWidget == nullptr) {
		UE_LOG(LogShippyMenu, Error, TEXT("InGameMenu is _not_ a UInGameMenu"));
		return;
	}
	InGameMenuWidget->SetInterface(InGameMenuInterface);
}

void UMenuSystem::MainMenuOpen(APlayerController &PlayerController)
{
	UE_LOG(LogShippyMenu, Log, TEXT("MainMenuOpen"));

	MainMenuWidget->AddToViewport();
	MainMenuWidget->SetVisibility(ESlateVisibility::Visible);

	MouseInputForMenu(PlayerController);
	FocusOnWidget(PlayerController, *MainMenuWidget, FName("HostButton"));
}

void UMenuSystem::MainMenuClose(APlayerController &PlayerController)
{
	if (MainMenuWidget == nullptr)
		return;
	UE_LOG(LogShippyMenu, Log, TEXT("MainMenuClose"));

	MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	MouseInputForGame(PlayerController);
}

void UMenuSystem::InGameMenuOpen(APlayerController &PlayerController)
{
	UE_LOG(LogShippyMenu, Log, TEXT("InGameMenuOpen"));

	InGameMenuWidget->AddToViewport();
	InGameMenuWidget->SetVisibility(ESlateVisibility::Visible);

	MouseInputForMenu(PlayerController);
	FocusOnWidget(PlayerController, *InGameMenuWidget, FName("CancelButton"));
}

void UMenuSystem::InGameMenuClose(APlayerController &PlayerController)
{
	if (InGameMenuWidget == nullptr)
		return;
	UE_LOG(LogShippyMenu, Log, TEXT("InGameMenuClose"));

	InGameMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	MouseInputForGame(PlayerController);
}

void UMenuSystem::SearchClearResults()
{
	if (MainMenuWidget == nullptr)
		return;
	MainMenuWidget->SearchClearResults();
}

void UMenuSystem::SearchAddServer(const FString& Name, const int SearchIndex)
{
	if (MainMenuWidget == nullptr)
		return;
	MainMenuWidget->SearchAddServer(Name, SearchIndex);
}

void UMenuSystem::MouseInputForMenu(APlayerController &PlayerController)
{
	PlayerController.bShowMouseCursor = true;

	FInputModeUIOnly inputMode;
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController.SetInputMode(inputMode);
}

void UMenuSystem::MouseInputForGame(APlayerController &PlayerController)
{
	PlayerController.bShowMouseCursor = false;

	FInputModeGameOnly inputMode;
	PlayerController.SetInputMode(inputMode);
}

void UMenuSystem::FocusOnWidget(APlayerController &PlayerController, UUserWidget& MenuWidget, const FName& Name)
{
	auto DefaultWidget = MenuWidget.GetWidgetFromName(Name);
	if (DefaultWidget == nullptr) {
		UE_LOG(LogShippyMenu, Warning, TEXT("UMenuSystem::FocusOnWidget - Widget %s was not found."), *Name.ToString())
		return;
	}

	DefaultWidget->SetUserFocus(&PlayerController);
	DefaultWidget->SetKeyboardFocus();
}
