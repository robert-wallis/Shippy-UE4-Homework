// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "LobbySystem.h"

#include "OnlineIdentityInterface.h"

#include "LogShippy.h"

DEFINE_LOG_CATEGORY(LogShippyLobby);
#define SESSION_NAME TEXT("Shippying It")

void ULobbySystem::Init(ILobbySystem* Interface)
{
	this->Interface = Interface;

	auto Online = IOnlineSubsystem::Get();
	if (Online == nullptr) {
		UE_LOG(LogShippyLobby, Error, TEXT("No Online Subsystem"));
		return;
	}

	auto OnlineSubsystemName = Online->GetSubsystemName();

	UE_LOG(LogShippyLobby, Log, TEXT("OnlineSubsystem: %s"), *OnlineSubsystemName.ToString());

	OnlineSession = Online->GetSessionInterface();
	if (!OnlineSession.IsValid()) {
		UE_LOG(LogShippyLobby, Warning, TEXT("Online Subsystem Interface Invalid :("));
		return;
	}

	IsLanMatch = OnlineSubsystemName == "NULL";

	auto IdSystem = Online->GetIdentityInterface();
	if (!IdSystem.IsValid()) {
		UE_LOG(LogShippyLobby, Error, TEXT("OnlineSubsystem IdentityInterface was null"));
		return;
	}
	PlayerId = IdSystem->GetUniquePlayerId(0);

	OnlineSession->OnCreateSessionCompleteDelegates.AddUObject(this, &ULobbySystem::OnSessionCreated);
	OnlineSession->OnFindSessionsCompleteDelegates.AddUObject(this, &ULobbySystem::OnSessionFindComplete);
	OnlineSession->OnJoinSessionCompleteDelegates.AddUObject(this, &ULobbySystem::OnSessionJoinComplete);
}

void ULobbySystem::SearchForServers()
{
	UE_LOG(LogShippy, Log, TEXT("UShippyGameInstance::MainMenuServerRefresh"));
	OnlineSessionSearch = MakeShareable(new FOnlineSessionSearch);
	OnlineSessionSearch->bIsLanQuery = IsLanMatch;
	OnlineSessionSearch->MaxSearchResults = 100;
	OnlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	OnlineSession->FindSessions(*PlayerId, OnlineSessionSearch.ToSharedRef());
}

void ULobbySystem::HostServer()
{
	SessionCreate();
}

void ULobbySystem::JoinServer(const int ServerIndex)
{
	if (!OnlineSessionSearch.IsValid()) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::MainMenuJoinServer(%d) OnlineSessionSearch is invalid"), ServerIndex);
		return;
	}
	if (!OnlineSessionSearch->SearchResults.IsValidIndex(ServerIndex)) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::MainMenuJoinServer(%d) ServerIndex > %d"), ServerIndex, OnlineSessionSearch->SearchResults.Num());
		return;
	}
	auto SessionSearchResult = OnlineSessionSearch->SearchResults[ServerIndex];
	OnlineSession->JoinSession(*PlayerId, SESSION_NAME, SessionSearchResult);
}

void ULobbySystem::QuitServer()
{
	SessionRemove(SESSION_NAME);
}

void ULobbySystem::SessionCreate()
{
	if (!OnlineSession.IsValid())
		return;

	SessionRemove(SESSION_NAME);

	UE_LOG(LogShippy, Log, TEXT("Creating Session: %s"), SESSION_NAME);

	auto IsLanMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = IsLanMatch;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	OnlineSession->CreateSession(*PlayerId, SESSION_NAME, SessionSettings);
}

void ULobbySystem::SessionRemove(const FName& SessionName)
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

void ULobbySystem::OnSessionCreated(const FName SessionName, bool Created)
{
	UE_LOG(LogShippyLobby, Log, TEXT("Session Created: %d, %s"), Created, *SessionName.ToString());
	Interface->LobbyHosted(Created);
}

void ULobbySystem::OnSessionFindComplete(bool WasSuccessful)
{
	UE_LOG(LogShippyLobby, Log, TEXT("Find Session Complete: %s"), WasSuccessful ? TEXT("success") : TEXT("error"));

	if (!OnlineSessionSearch.IsValid())
		return;

	auto Results = TArray<FLobbyServer>();
	for (auto Result : OnlineSessionSearch->SearchResults) {
		FLobbyServer Server;
		Server.Name = Result.GetSessionIdStr();
		Results.Add(Server);
	}

	Interface->LobbySearchComplete(Results);
}

void ULobbySystem::OnSessionJoinComplete(const FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success) {
		UE_LOG(LogShippyLobby, Error, TEXT("Couldn't join %s: EOnJoinSessionCompleteResult#%d"), *SessionName.ToString(), Result);
		return;
	}

	FString ConnectString;
	if (!OnlineSession->GetResolvedConnectString(SessionName, ConnectString)) {
		UE_LOG(LogShippyLobby, Error, TEXT("OnSessionJoinComplete Couldn't resolve connect string %s"), *SessionName.ToString());
		return;
	}

	Interface->LobbyJoined(Result == EOnJoinSessionCompleteResult::Success, ConnectString);
}
