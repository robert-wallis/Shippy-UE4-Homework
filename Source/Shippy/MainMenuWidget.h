// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuInterface.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHIPPY_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()


	IMainMenuInterface* MenuInterface;

public:

	bool Initialize() override;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	//UPROPERTY(meta = (BindWidget))
	//class UEditableTextBox* JoinAddressEdit;

	void SetMenuInterface(IMainMenuInterface* mainMenuInterface);


private:

	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnJoinClicked();

};
