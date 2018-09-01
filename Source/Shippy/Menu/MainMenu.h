// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuInterface.h"
#include "ServerRowInterface.h"
#include "../LobbyServer.h"
#include "MainMenu.generated.h"

/**
 * Bindings for the Main Menu blueprint widget.
 */
UCLASS()
class SHIPPY_API UMainMenu : public UUserWidget, public ServerRowInterface
{
	GENERATED_BODY()

	TSubclassOf<class UUserWidget> ServerRowClass;

	MainMenuInterface* Interface;

public:

	UPROPERTY(meta = ( BindWidget ))
	class UWidgetSwitcher* Switcher;

	UPROPERTY(meta = ( BindWidget ))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;

	UPROPERTY(meta = ( BindWidget ))
	class UEditableTextBox* HostServerName;

	UPROPERTY(meta = ( BindWidget ))
	class UButton* HostButton;

	UPROPERTY(meta = ( BindWidget ))
	class UButton* HostBackButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SearchButton;

	UPROPERTY(meta = ( BindWidget ))
	class UWidget* SearchMenu;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* SearchList;

	UPROPERTY(meta = (BindWidget))
	class UButton* SearchBackButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* AddressTextBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreditsButton;

	UPROPERTY(meta = ( BindWidget ))
	class UWidget* CreditsMenu;

	UPROPERTY(meta = ( BindWidget ))
	class UButton* CreditsBackButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UMainMenu(const FObjectInitializer& ObjectInitializer);

	bool Initialize() override;

	void SetInterface(MainMenuInterface* Interface);

	UFUNCTION()
	void SearchClearResults();

	UFUNCTION()
	void SearchAddServer(const FLobbyServer& Name, const int SearchIndex);

private:

	UFUNCTION()
	void OnBackToMainMenu();

	UFUNCTION()
	void OnHostMenuClicked();

	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnSearchClicked();

	UFUNCTION()
	void OnSearchBackClicked();

	UFUNCTION()
	void ServerRowJoin(const int SearchIndex) override;

	UFUNCTION()
	void OnJoinClicked();

	UFUNCTION()
	void OnCreditsClicked();

	UFUNCTION()
	void OnQuitClicked();
};

