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
#include "LobbySystem.h"
#include "ShippyGameMode.h"
#include "LogShippy.h"


UShippyGameInstance::UShippyGameInstance(const FObjectInitializer &ObjectInitializer)
{
	MenuSystem = ObjectInitializer.CreateDefaultSubobject<UMenuSystem>(this, TEXT("MenuSystem"));
	LobbySystem = ObjectInitializer.CreateDefaultSubobject<ULobbySystem>(this, TEXT("LobbySystem"));
}

void UShippyGameInstance::Init()
{
	LobbySystem->Init(this);
	MenuSystem->Init(this, this, this);
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

void UShippyGameInstance::MainMenuHost(const FName& SessionName)
{
	ClientMessage("Hosting Game");
	LobbySystem->HostServer(SessionName);
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
	LobbySystem->JoinServer(ServerIndex);
}

void UShippyGameInstance::MainMenuQuit()
{
	ClientMessage("MainMenu Quit");
	GetFirstGamePlayer()->ConsoleCommand("QUIT");
}

void UShippyGameInstance::MainMenuServerRefresh()
{
	LobbySystem->SearchForServers();
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

void UShippyGameInstance::StartSession()
{
	LobbySystem->StartSession();
}

bool UShippyGameInstance::IsSeamlessTravelSupported()
{
	if (LobbySystem == nullptr) {
		return false;
	}
	return LobbySystem->UseSeamlessTravel;
}

void UShippyGameInstance::InGameMenuExitToMainMenu()
{
	auto PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	LobbySystem->QuitServer();

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

void UShippyGameInstance::ClientMessage(const FString & Message)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, Message);
	UE_LOG(LogShippy, Log, TEXT("%s"), *Message);
	auto PlayerController = GetFirstLocalPlayerController();
	if (PlayerController != nullptr) {
		PlayerController->ClientMessage(Message);
	}
}

void UShippyGameInstance::LobbySearchComplete(const TArray<FLobbyServer>& Sessions)
{
	MenuSystem->SearchClearResults();

	for (int i = 0; i < Sessions.Num(); i++) {
		auto Server = Sessions[i];
		UE_LOG(LogShippy, Log, TEXT("Found Session %d: %s"), i, *Server.Name);
		MenuSystem->SearchAddServer(Server, i);
	}
}

void UShippyGameInstance::LobbyHosted(const bool Success)
{
	if (!Success) {
		MainMenu();
		return;
	}

	auto GameMode = GetWorld()->GetAuthGameMode();
	if (GameMode == nullptr)
		return;

	auto ShippyGameMode = Cast<AShippyGameMode>(GameMode);
	if (ShippyGameMode == nullptr) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::LobbyHosted current game mode is not An AShippyGameMode, so we can't travel to lobby"));
		return;
	}

	ShippyGameMode->TravelToLobby();

	auto PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr) {
		UE_LOG(LogShippy, Error, TEXT("UShippyGameInstance::LobbyHosted PlayerController null, cant close menu"));
		return;
	}
	MenuSystem->MainMenuClose(*PlayerController);
}

void UShippyGameInstance::LobbyJoined(const bool Success, const FString& ConnectString)
{
	if (!Success) {
		return;
	}
	MainMenuJoinGame(ConnectString);
}
