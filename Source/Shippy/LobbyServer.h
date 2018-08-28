// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LobbyServer.generated.h"

USTRUCT()
struct FLobbyServer
{
	GENERATED_BODY()

	FString Name;
	FString UserName;
	uint32 PingInMs;
};
