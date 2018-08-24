// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "ServerRow.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "../LogShippy.h"


bool UServerRow::Initialize()
{
	if (!Super::Initialize())
		return false;

	if (JoinButton == nullptr)
		return false;

	if (ServerNameText == nullptr)
		return false;

	JoinButton->OnClicked.AddDynamic(this, &UServerRow::OnJoinClicked);

	return true;
}

void UServerRow::SetInterface(ServerRowInterface* Interface)
{
	this->Interface = Interface;
}

void UServerRow::SetServer(const FString& Name, const FString& Address)
{
	ServerNameText->SetText(FText::FromString(Name));
	this->Address = Address;
}

void UServerRow::OnJoinClicked()
{
	if (this->Interface == nullptr) {
		UE_LOG(LogShippy, Warning, TEXT("UServerRow::OnJoinClicked() Interface is null"));
		return;
	}

	this->Interface->ServerRowJoin(Address);
}
