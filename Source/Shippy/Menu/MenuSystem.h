// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "LobbyServer.h"
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

	void Init(UGameInstance* GameInstance, class MainMenuInterface* MainMenuInterface, class InGameMenuInterface* InGameMenuInterface);

	void MainMenuOpen(class APlayerController &PlayerController);
	void MainMenuClose(class APlayerController &PlayerController);

	void InGameMenuOpen(class APlayerController &PlayerController);
	void InGameMenuClose(class APlayerController &PlayerController);

	UFUNCTION()
	void SearchClearResults();

	UFUNCTION()
	void SearchAddServer(const FLobbyServer& Server, const int SearchIndex);

private:

	void MouseInputForMenu(class APlayerController &PlayerController);
	void MouseInputForGame(class APlayerController &PlayerController);
	void FocusOnWidget(class APlayerController &PlayerController, class UUserWidget& MenuWidget, const FName& Widget);
};
