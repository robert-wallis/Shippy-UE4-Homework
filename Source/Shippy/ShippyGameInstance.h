// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSessionInterface.h"

#include "Menu/MainMenuInterface.h"
#include "Menu/InGameMenuInterface.h"
#include "ShippyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHIPPY_API UShippyGameInstance : 
	public UGameInstance,
	public MainMenuInterface,
	public InGameMenuInterface
{
	GENERATED_BODY()

private:

	IOnlineSessionPtr OnlineSession;
	TSharedPtr<FOnlineSessionSearch> OnlineSessionSearch;

	UPROPERTY()
	class UMenuSystem *MenuSystem;

public:

	UShippyGameInstance(const FObjectInitializer &ObjectInitializer);
	
	void Init() override;

	UFUNCTION(BlueprintCallable)
	void MainMenu();

	UFUNCTION()
	void InGameMenu();

private:

	void InitOnlineSubsystem();

	UFUNCTION()
	void MainMenuHost() override;

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

	void SessionCreate();
	void SessionRemove(const FName& SessionName);
	void OnSessionCreated(const FName SessionName, bool Created);
	void OnSessionFindComplete(bool WasSuccessful);
	void OnSessionJoinComplete(const FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void ClientMessage(const FString& Message);

};
