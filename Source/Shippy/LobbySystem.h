// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSessionInterface.h"

#include "LobbySystemInterface.h"
#include "LobbySystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogShippyLobby, Log, All);

/**
 * Connecting players over the internet.
 */
UCLASS()
class SHIPPY_API ULobbySystem : public UObject
{
	GENERATED_BODY()

private:
	ILobbySystem* Interface;
	TSharedPtr<const FUniqueNetId> PlayerId;
	IOnlineSessionPtr OnlineSession;
	TSharedPtr<FOnlineSessionSearch> OnlineSessionSearch;
	bool IsLanMatch;

public:

	void Init(ILobbySystem* Interface);
	void SearchForServers();
	void HostServer();
	void JoinServer(const int ServerIndex);
	void QuitServer();
	
private:

	void SessionCreate();
	void SessionRemove(const FName& SessionName);

	void OnSessionCreated(const FName SessionName, bool Created);
	void OnSessionFindComplete(bool WasSuccessful);
	void OnSessionJoinComplete(const FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
};


