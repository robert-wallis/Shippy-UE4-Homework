// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Interface for MainMenu pass messages to it's owner.
 */
class SHIPPY_API MainMenuInterface
{
public:
	
	virtual void MainMenuHost() = 0;
	virtual void MainMenuJoinGame(const FString &Address) = 0;
	virtual void MainMenuJoinServer(const int SearchIndex) = 0;
	virtual void MainMenuQuit() = 0;
	virtual void MainMenuServerRefresh() = 0;
	
};
