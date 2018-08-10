// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "ShippyGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSessionInterface.h"
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
	InitOnlineSubsystem();
	InitMenus();
}

void UShippyGameInstance::InitOnlineSubsystem()
{
	auto subsystem = IOnlineSubsystem::Get();
	if (subsystem == nullptr) {
		UE_LOG(LogShippy, Warning, TEXT("No Online Subsystem"));
		return;
	}

	UE_LOG(LogShippy, Log, TEXT("OnlineSubsystem: %s"), *subsystem->GetSubsystemName().ToString());

	OnlineSession = subsystem->GetSessionInterface();
	if (!OnlineSession.IsValid()) {
		UE_LOG(LogShippy, Warning, TEXT("Online Subsystem Interface Invalid :("));
		return;
	}

	OnlineSession->OnCreateSessionCompleteDelegates.AddUObject(this, &UShippyGameInstance::OnSessionCreated);
}

void UShippyGameInstance::InitMenus()
{
	MainMenuWidget = CreateWidget<UMainMenu>(this, MainMenuClass);
	if (MainMenuWidget == nullptr) {
		UE_LOG(LogShippy, Warning, TEXT("MainMenu is _not_ a UMainMenu"));
		return;
	}
	MainMenuWidget->SetInterface(this);

	InGameMenuWidget = CreateWidget<UInGameMenu>(this, InGameMenuClass);
	if (InGameMenuWidget == nullptr) {
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
	MainMenuWidget->SetVisibility(ESlateVisibility::Visible);

	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	MouseInputForMenu(*playerController);
	FocusOnWidget(*MainMenuWidget, *playerController, FName("HostButton"));
}

void UShippyGameInstance::MainMenuHost()
{
	ClientMessage("Hosting Game");
	MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	MouseInputForGame();
	SessionCreate();
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
		MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
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
	InGameMenuWidget->SetVisibility(ESlateVisibility::Visible);

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

	auto sessionName = TEXT("Shippying It");
	SessionRemove(sessionName);

	InGameMenuWidget->SetVisibility(ESlateVisibility::Hidden);

	ClientMessage("Left Game");
	playerController->ClientTravel("/Game/Menu/MainMenu", ::TRAVEL_Relative);
}

void UShippyGameInstance::InGameMenuCancel()
{
	if (InGameMenuWidget == nullptr)
		return;
	InGameMenuWidget->SetVisibility(ESlateVisibility::Hidden);
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

void UShippyGameInstance::SessionCreate()
{
	if (!OnlineSession.IsValid())
		return;

	auto sessionName = TEXT("Shippying It");
	SessionRemove(sessionName);

	UE_LOG(LogShippy, Log, TEXT("Creating Session: %s"), sessionName);
	FOnlineSessionSettings sessionSettings;
	OnlineSession->CreateSession(0, sessionName, sessionSettings);
}

void UShippyGameInstance::SessionRemove(const FName& sessionName)
{
	if (!OnlineSession.IsValid())
		return;

	auto namedOnlineSession = OnlineSession->GetNamedSession(sessionName);
	if (namedOnlineSession != nullptr) {
		UE_LOG(LogShippy, Log, TEXT("Closing existing session: %s"), *sessionName.ToString());
		OnlineSession->RemoveNamedSession(sessionName);
		OnlineSession->DestroySession(sessionName);
	}
}

void UShippyGameInstance::OnSessionCreated(const FName sessionName, bool created)
{
	UE_LOG(LogShippy, Log, TEXT("Session Created: %d, %s"), created, *sessionName.ToString());
	if (!created) {
		MainMenu();
		return;
	}
	GetWorld()->ServerTravel("/Game/Platform/Maps/PuzzleRoom?listen");
}

void UShippyGameInstance::ClientMessage(const FString & message)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, message);
	auto playerController = GetFirstLocalPlayerController();
	if (playerController != nullptr) {
		playerController->ClientMessage(message);
	}
}
