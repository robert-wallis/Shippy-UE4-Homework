// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Menu/MainMenu.h"
#include "Menu/InGameMenu.h"
#include "MenuSystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogShippyMenu, Log, All);

/**
 * Controls and binds the game menus.
 */
UCLASS()
class SHIPPY_API UMenuSystem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class UMainMenu* MainMenuWidget;

	UPROPERTY()
	class UInGameMenu* InGameMenuWidget;

private:

	TSubclassOf<class UUserWidget> MainMenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;

public:

	UMenuSystem(const FObjectInitializer &ObjectInitializer);

	void Init(UGameInstance* gameInstance, class MainMenuInterface* mainMenuInterface, class InGameMenuInterface* inGameMenuInterface);

	void MainMenuOpen(class APlayerController &playerController);
	void MainMenuClose(class APlayerController &playerController);

	void InGameMenuOpen(class APlayerController &playerController);
	void InGameMenuClose(class APlayerController &playerController);

	UFUNCTION()
	void SearchClearResults();

	UFUNCTION()
	void SearchAddServer(const FString& Name, const int SearchIndex);

private:

	void MouseInputForMenu(class APlayerController &playerController);
	void MouseInputForGame(class APlayerController &playerController);
	void FocusOnWidget(class APlayerController &playerController, class UUserWidget& menuWidget, const FName& Widget);
};
