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
	auto SubSystem = IOnlineSubsystem::Get();
	if (SubSystem == nullptr) {
		UE_LOG(LogShippy, Warning, TEXT("No Online Subsystem"));
		return;
	}

	UE_LOG(LogShippy, Log, TEXT("OnlineSubsystem: %s"), *SubSystem->GetSubsystemName().ToString());

	OnlineSession = SubSystem->GetSessionInterface();
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
	auto PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::MainMenu PlayerController is null"));
		return;
	}
	MenuSystem->MainMenuOpen(*PlayerController);
}

void UShippyGameInstance::MainMenuHost()
{
	ClientMessage("Hosting Game");
	SessionCreate();
}

void UShippyGameInstance::MainMenuJoinGame(const FString& Address)
{
	const auto Message = FString::Printf(TEXT("Joining %s"), *Address);
	ClientMessage(Message);

	auto PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr) {
		return;
	}
	MenuSystem->MainMenuClose(*PlayerController);

	PlayerController->ClientTravel(Address, ::TRAVEL_Absolute);
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
	auto PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::InGameMenu PlayerController is null"));
		return;
	}
	MenuSystem->InGameMenuOpen(*PlayerController);
}

void UShippyGameInstance::InGameMenuExitToMainMenu()
{
	auto PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	SessionRemove(SESSION_NAME);

	ClientMessage("Left Game");
	PlayerController->ClientTravel("/Game/Menu/MainMenu", ::TRAVEL_Relative);
}

void UShippyGameInstance::InGameMenuCancel()
{
	auto PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;
	MenuSystem->InGameMenuClose(*PlayerController);
}


void UShippyGameInstance::SessionCreate()
{
	if (!OnlineSession.IsValid())
		return;

	SessionRemove(SESSION_NAME);

	UE_LOG(LogShippy, Log, TEXT("Creating Session: %s"), SESSION_NAME);
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	auto PlayerId = GetPrimaryPlayerUniqueId();
	OnlineSession->CreateSession(*PlayerId, SESSION_NAME, SessionSettings);
}

void UShippyGameInstance::SessionRemove(const FName& SessionName)
{
	if (!OnlineSession.IsValid())
		return;

	auto NamedSession = OnlineSession->GetNamedSession(SessionName);
	if (NamedSession != nullptr) {
		UE_LOG(LogShippy, Log, TEXT("Closing existing session: %s"), *SessionName.ToString());
		OnlineSession->RemoveNamedSession(SessionName);
		OnlineSession->DestroySession(SessionName);
	}
}

void UShippyGameInstance::OnSessionCreated(const FName SessionName, bool Created)
{
	UE_LOG(LogShippy, Log, TEXT("Session Created: %d, %s"), Created, *SessionName.ToString());
	if (!Created) {
		MainMenu();
		return;
	}
	auto PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::OnSessionCreated PlayerController null, cant close menu"));
		return;
	}
	if (!GetWorld()->ServerTravel("/Game/Platform/Maps/PuzzleRoom?listen")) {
		ClientMessage(TEXT("Error Joining Server"));
		return;
	}
	MenuSystem->MainMenuClose(*PlayerController);
}

void UShippyGameInstance::OnSessionFindComplete(bool WasSuccessful)
{
	UE_LOG(LogShippy, Log, TEXT("Find Session Complete: %s"), WasSuccessful ? TEXT("success") : TEXT("error"));
	MenuSystem->SearchClearResults();

	if (!OnlineSessionSearch.IsValid())
		return;

	for (int i = 0; i < OnlineSessionSearch->SearchResults.Num(); i++) {
		auto SessionResult = OnlineSessionSearch->SearchResults[i];
		UE_LOG(LogShippy, Log, TEXT("Found Session %d: %s"), i, *SessionResult.GetSessionIdStr());
		
		MenuSystem->SearchAddServer(SessionResult.GetSessionIdStr(), i);
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

void UShippyGameInstance::ClientMessage(const FString & Message)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, Message);
	UE_LOG(LogShippy, Log, TEXT("%s"), *Message);
	auto PlayerController = GetFirstLocalPlayerController();
	if (PlayerController != nullptr) {
		PlayerController->ClientMessage(Message);
	}
}
