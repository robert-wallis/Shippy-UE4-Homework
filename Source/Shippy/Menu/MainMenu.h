// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuInterface.h"
#include "MainMenu.generated.h"

/**
 * Bindings for the Main Menu blueprint widget.
 */
UCLASS()
class SHIPPY_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

	MainMenuInterface* Interface;

public:

	bool Initialize() override;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* AddressTextBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreditsButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreditsBackButton;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* Switcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* CreditsMenu;

	void SetInterface(MainMenuInterface* Interface);


private:

	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnJoinClicked();

	UFUNCTION()
	void OnBackToMainMenu();

	UFUNCTION()
	void OnCreditsClicked();

};

