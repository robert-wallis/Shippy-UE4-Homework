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

#include "Menu/MenuSystem.h"
#include "LogShippy.h"

UShippyGameInstance::UShippyGameInstance(const FObjectInitializer &ObjectInitializer)
{
	MenuSystem = ObjectInitializer.CreateDefaultSubobject<UMenuSystem>(this, TEXT("MenuSystem"));
}

void UShippyGameInstance::Init()
{
	InitOnlineSubsystem();
	MenuSystem->Init(this, this, this);
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
	OnlineSession->OnFindSessionsCompleteDelegates.AddUObject(this, &UShippyGameInstance::OnSessionFindComplete);

}

void UShippyGameInstance::MainMenu()
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::MainMenu PlayerController is null"));
		return;
	}
	MenuSystem->MainMenuOpen(*playerController);
}

void UShippyGameInstance::MainMenuHost()
{
	ClientMessage("Hosting Game");
	SessionCreate();
}

void UShippyGameInstance::MainMenuJoinGame(const FString& address)
{
	const auto message = FString::Printf(TEXT("Joining %s"), *address);
	ClientMessage(message);

	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	MenuSystem->MainMenuClose(*playerController);

	playerController->ClientTravel(address, ::TRAVEL_Relative);
}

void UShippyGameInstance::MainMenuQuit()
{
	ClientMessage("MainMenu Quit");
	GetFirstGamePlayer()->ConsoleCommand("QUIT");
}

void UShippyGameInstance::MainMenuServerRefresh()
{
	UE_LOG(LogShippy, Log, TEXT("UShippyGameInstance::MainMenuServerRefresh"));
	OnlineSessionSearch = MakeShareable(new FOnlineSessionSearch);
	OnlineSessionSearch->bIsLanQuery = true;
	OnlineSession->FindSessions(0, OnlineSessionSearch.ToSharedRef());
}

void UShippyGameInstance::InGameMenu()
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::InGameMenu PlayerController is null"));
		return;
	}
	MenuSystem->InGameMenuOpen(*playerController);
}

void UShippyGameInstance::InGameMenuExitToMainMenu()
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;

	auto sessionName = TEXT("Shippying It");
	SessionRemove(sessionName);

	ClientMessage("Left Game");
	playerController->ClientTravel("/Game/Menu/MainMenu", ::TRAVEL_Relative);
}

void UShippyGameInstance::InGameMenuCancel()
{
	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr)
		return;
	MenuSystem->InGameMenuClose(*playerController);
}


void UShippyGameInstance::SessionCreate()
{
	if (!OnlineSession.IsValid())
		return;

	auto sessionName = TEXT("Shippying It");
	SessionRemove(sessionName);

	UE_LOG(LogShippy, Log, TEXT("Creating Session: %s"), sessionName);
	FOnlineSessionSettings sessionSettings;
	sessionSettings.bIsLANMatch = true;
	sessionSettings.NumPublicConnections = 2;
	sessionSettings.bShouldAdvertise = true;
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
	auto playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::OnSessionCreated PlayerController null, cant close menu"));
		return;
	}
	MenuSystem->MainMenuClose(*playerController);
	GetWorld()->ServerTravel("/Game/Platform/Maps/PuzzleRoom?listen");
}

void UShippyGameInstance::OnSessionFindComplete(bool wasSuccessful)
{
	UE_LOG(LogShippy, Log, TEXT("Find Session Complete: %s"), wasSuccessful ? TEXT("success") : TEXT("error"));
	MenuSystem->SearchClearResults();
	if (!OnlineSessionSearch.IsValid())
		return;
	for (auto res : OnlineSessionSearch->SearchResults) {
		UE_LOG(LogShippy, Log, TEXT("Found Session: %s"), *res.GetSessionIdStr());
		MenuSystem->SearchAddServer(res.GetSessionIdStr(), res.GetSessionIdStr());
	}
}

void UShippyGameInstance::ClientMessage(const FString & message)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, message);
	auto playerController = GetFirstLocalPlayerController();
	if (playerController != nullptr) {
		playerController->ClientMessage(message);
	}
}
