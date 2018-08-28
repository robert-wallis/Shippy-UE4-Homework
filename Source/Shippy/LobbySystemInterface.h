// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LobbyServer.h"

class ILobbySystem
{
public:

	virtual void LobbySearchComplete(const TArray<FLobbyServer>& Sessions) = 0;
	virtual void LobbyHosted(const bool Success) = 0;
	virtual void LobbyJoined(const bool Success, const FString& ConnectString) = 0;

};