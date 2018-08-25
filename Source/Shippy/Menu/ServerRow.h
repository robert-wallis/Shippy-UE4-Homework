// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRowInterface.h"
#include "ServerRow.generated.h"

/**
 * A single server in the server search menu.
 */
UCLASS()
class SHIPPY_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

	int SearchIndex;
	ServerRowInterface* Interface;
	
public:

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerNameText;

	bool Initialize() override;

	void SetInterface(ServerRowInterface* Interface);

	void SetServer(const FString& Name, const int SearchIndex);

	UFUNCTION()
	void OnJoinClicked();

};
