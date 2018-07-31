// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Menu/MainMenu.h"
#include "Menu/InGameMenu.h"
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

	TSubclassOf<class UUserWidget> MainMenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;

	UPROPERTY()
	class UMainMenu* MainMenuWidget;

	UPROPERTY()
	class UInGameMenu* InGameMenuWidget;

public:

	UShippyGameInstance(const FObjectInitializer &ObjectInitializer);
	
	void Init() override;

	UFUNCTION(Exec, BlueprintCallable, Category=Menu)
	void MainMenu();

	UFUNCTION(Exec, BlueprintCallable, Category=Menu)
	void InGameMenu();

private:

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

	void EnableMouseControl(class APlayerController& playerController);

	void FocusOnWidget(class UUserWidget& menuWidget, class APlayerController& playerController, const FName& Widget);

	void ClientMessage(const FString& message);

};
