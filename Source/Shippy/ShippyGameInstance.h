// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Menu/MainMenuInterface.h"
#include "Menu/InGameMenuInterface.h"
#include "LobbySystemInterface.h"
#include "ShippyGameInstance.generated.h"

/**
 * Game Logic
 */
UCLASS()
class SHIPPY_API UShippyGameInstance : 
	public UGameInstance,
	public MainMenuInterface,
	public InGameMenuInterface,
	public ILobbySystem
{
	GENERATED_BODY()

private:

	UPROPERTY()
	class UMenuSystem *MenuSystem;

	UPROPERTY()
	class ULobbySystem *LobbySystem;

public:

	UShippyGameInstance(const FObjectInitializer &ObjectInitializer);
	
	void Init() override;

	UFUNCTION(BlueprintCallable)
	void MainMenu();

	UFUNCTION()
	void InGameMenu();

	UFUNCTION()
	bool IsSeamlessTravelSupported();

private:

	UFUNCTION()
	void MainMenuHost(const FName& SessionName) override;

	UFUNCTION()
	void MainMenuJoinGame(const FString& Address) override;

	UFUNCTION()
	void MainMenuJoinServer(const int ServerIndex) override;

	UFUNCTION()
	void MainMenuQuit() override;

	UFUNCTION()
	void MainMenuServerRefresh() override;

	UFUNCTION()
	void InGameMenuExitToMainMenu() override;

	UFUNCTION()
	void InGameMenuCancel() override;

	virtual void LobbySearchComplete(const TArray<FLobbyServer>& Sessions) override;
	virtual void LobbyHosted(const bool Success) override;
	virtual void LobbyJoined(const bool Success, const FString& ConnectString) override;

	void ClientMessage(const FString& Message);

};
