// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRowInterface.h"
#include "LobbyServer.h"
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

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PingText;

	void Init(ServerRowInterface* Interface, const FLobbyServer& Server, const int SearchIndex);

	UFUNCTION()
	void OnJoinClicked();

};
