// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainMenuInterface.h"
#include "InGameMenuInterface.h"
#include "ShippyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHIPPY_API UShippyGameInstance : 
	public UGameInstance,
	public IMainMenuInterface,
	public IInGameMenuInterface
{
	GENERATED_BODY()

private:

	TSubclassOf<class UUserWidget> MainMenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;

	UPROPERTY()
	class UMainMenuWidget* MainMenuWidget;

	UPROPERTY()
	class UInGameMenu* InGameMenuWidget;

public:

	UShippyGameInstance(const FObjectInitializer &ObjectInitializer);
	
	virtual void Init() override;

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
	void InGameMenuExitToMainMenu() override;

	UFUNCTION()
	void InGameMenuCancel() override;

	void EnableMouseControl(class APlayerController& playerController);

	void FocusOnWidget(class UUserWidget& menuWidget, class APlayerController& playerController, const FName& Widget);

};
