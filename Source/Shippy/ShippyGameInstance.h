// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainMenuInterface.h"
#include "ShippyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHIPPY_API UShippyGameInstance : public UGameInstance, public IMainMenuInterface
{
	GENERATED_BODY()

private:

	TSubclassOf<class UUserWidget> MainMenuClass;

public:

	UShippyGameInstance(const FObjectInitializer &ObjectInitializer);
	
	virtual void Init() override;

	UFUNCTION(Exec, BlueprintCallable, Category=Menu)
	void MainMenu();

	UFUNCTION()
	void MainMenuHost();

	UFUNCTION()
	void MainMenuJoinGame(const FString& Address);

};
