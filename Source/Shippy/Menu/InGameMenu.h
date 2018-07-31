// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenuInterface.h"
#include "InGameMenu.generated.h"

/**
 * Main Menu that shows while playing the game.
 */
UCLASS()
class SHIPPY_API UInGameMenu : public UUserWidget
{
	GENERATED_BODY()

private:

	InGameMenuInterface* Interface;

public:

	UPROPERTY(meta = (BindWidget))
	class UButton* ExitToMainMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;

	bool Initialize() override;

	void SetInterface(InGameMenuInterface* Interface);

private:

	UFUNCTION()
	void OnExitToMainMenuClicked();

	UFUNCTION()
	void OnCancelClicked();

};
