// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "LobbyGameMode.h"

#include "ShippyGameInstance.h"
#include "LogShippy.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	PlayerCount++;

	if (PlayerCount >= 2) {
		CountDownToTravel();
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	PlayerCount--;
}
