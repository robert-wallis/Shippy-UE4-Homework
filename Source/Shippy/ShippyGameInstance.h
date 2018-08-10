// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"

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

	UPROPERTY()
	class UMenuSystem *MenuSystem;

public:

	UShippyGameInstance(const FObjectInitializer &ObjectInitializer);
	
	void Init() override;

	UFUNCTION(Exec, BlueprintCallable, Category=Menu)
	void MainMenu();

	UFUNCTION(Exec, BlueprintCallable, Category=Menu)
	void InGameMenu();

private:

	void InitOnlineSubsystem();

	UFUNCTION()
	void MainMenuHost() override;

	UFUNCTION()
	void MainMenuJoinGame(const FString& Address) override;

	UFUNCTION()
	void MainMenuQuit() override;

	UFUNCTION()
	void InGameMenuExitToMainMenu() override;

	UFUNCTION()
	void InGameMenuCancel() override;

	void SessionCreate();
	void SessionRemove(const FName& sessionName);
	void OnSessionCreated(const FName sessionName, bool created);

	void ClientMessage(const FString& message);

};
