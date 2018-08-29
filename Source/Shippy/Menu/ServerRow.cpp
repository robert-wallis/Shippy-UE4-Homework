// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "ServerRow.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "../LogShippy.h"


void UServerRow::Init(ServerRowInterface* InterfaceParam, const FLobbyServer& Server, const int SearchIndexParam)
{
	this->Interface = InterfaceParam;
	ServerNameText->SetText(FText::FromString(Server.Name));
	HostNameText->SetText(FText::FromString(Server.UserName));
	PingText->SetText(FText::FromString(FString::Printf(TEXT("%d ms"), Server.PingInMs)));
	this->SearchIndex = SearchIndexParam;
	JoinButton->OnClicked.AddDynamic(this, &UServerRow::OnJoinClicked);
}

void UServerRow::OnJoinClicked()
{
	if (this->Interface == nullptr) {
		UE_LOG(LogShippy, Warning, TEXT("UServerRow::OnJoinClicked() Interface is null"));
		return;
	}

	this->Interface->ServerRowJoin(SearchIndex);
}
