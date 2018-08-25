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

#include "Runtime/Engine/Classes/GameFramework/PlayerState.h"

#include "Menu/MenuSystem.h"
#include "LogShippy.h"

#define SESSION_NAME TEXT("Shippying It")

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
	OnlineSession->OnJoinSessionCompleteDelegates.AddUObject(this, &UShippyGameInstance::OnSessionJoinComplete);
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

	playerController->ClientTravel(address, ::TRAVEL_Absolute);
}

void UShippyGameInstance::MainMenuJoinServer(const int ServerIndex)
{
	if (!OnlineSessionSearch.IsValid()) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::MainMenuJoinServer(%d) OnlineSessionSearch is invalid"), ServerIndex);
		return;
	}
	if (!OnlineSessionSearch->SearchResults.IsValidIndex(ServerIndex)) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::MainMenuJoinServer(%d) ServerIndex > %d"), ServerIndex, OnlineSessionSearch->SearchResults.Num());
		return;
	}
	auto PlayerId = GetPrimaryPlayerUniqueId();
	auto SessionSearchResult = OnlineSessionSearch->SearchResults[ServerIndex];
	OnlineSession->JoinSession(*PlayerId, SESSION_NAME, SessionSearchResult);
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
	OnlineSessionSearch->bIsLanQuery = false;
	OnlineSessionSearch->MaxSearchResults = 100;
	OnlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	auto PlayerId = GetPrimaryPlayerUniqueId();
	OnlineSession->FindSessions(*PlayerId, OnlineSessionSearch.ToSharedRef());
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

	SessionRemove(SESSION_NAME);

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

	SessionRemove(SESSION_NAME);

	UE_LOG(LogShippy, Log, TEXT("Creating Session: %s"), SESSION_NAME);
	FOnlineSessionSettings sessionSettings;
	sessionSettings.bIsLANMatch = false;
	sessionSettings.NumPublicConnections = 2;
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.bUsesPresence = true;
	auto PlayerId = GetPrimaryPlayerUniqueId();
	OnlineSession->CreateSession(*PlayerId, SESSION_NAME, sessionSettings);
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
	if (!GetWorld()->ServerTravel("/Game/Platform/Maps/PuzzleRoom?listen")) {
		ClientMessage(TEXT("Error Joining Server"));
		return;
	}
	MenuSystem->MainMenuClose(*playerController);
}

void UShippyGameInstance::OnSessionFindComplete(bool wasSuccessful)
{
	UE_LOG(LogShippy, Log, TEXT("Find Session Complete: %s"), wasSuccessful ? TEXT("success") : TEXT("error"));
	MenuSystem->SearchClearResults();

	if (!OnlineSessionSearch.IsValid())
		return;

	for (int i = 0; i < OnlineSessionSearch->SearchResults.Num(); i++) {
		auto res = OnlineSessionSearch->SearchResults[i];
		UE_LOG(LogShippy, Log, TEXT("Found Session %d: %s"), i, *res.GetSessionIdStr());
		
		MenuSystem->SearchAddServer(res.GetSessionIdStr(), i);
	}
}

void UShippyGameInstance::OnSessionJoinComplete(const FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success) {
		UE_LOG(LogShippy, Error, TEXT("Couldn't join %s: EOnJoinSessionCompleteResult#%d"), *SessionName.ToString(), Result);
		return;
	}

	FString TravelURL;
	if (!OnlineSession->GetResolvedConnectString(SessionName, TravelURL)) {
		UE_LOG(LogShippy, Error, TEXT("OnSessionJoinComplete Couldn't resolve connect string %s"), *SessionName.ToString());
		return;
	}

	MainMenuJoinGame(TravelURL);
}

void UShippyGameInstance::ClientMessage(const FString & message)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, message);
	UE_LOG(LogShippy, Log, TEXT("%s"), *message);
	auto playerController = GetFirstLocalPlayerController();
	if (playerController != nullptr) {
		playerController->ClientMessage(message);
	}
}
