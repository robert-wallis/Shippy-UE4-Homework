// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "InGameMenu.h"
#include "Components/Button.h"

bool UInGameMenu::Initialize()
{
	if (!Super::Initialize())
		return false;

	if (ExitToMainMenuButton == nullptr)
		return false;
	if (CancelButton == nullptr)
		return false;

	ExitToMainMenuButton->OnClicked.AddDynamic(this, &UInGameMenu::OnExitToMainMenuClicked);
	CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::OnCancelClicked);

	return true;
}

void UInGameMenu::SetInterface(InGameMenuInterface* InterfaceParam)
{
	this->Interface = InterfaceParam;
}

void UInGameMenu::OnExitToMainMenuClicked()
{
	if (Interface == nullptr)
		return;

	Interface->InGameMenuExitToMainMenu();
}

void UInGameMenu::OnCancelClicked()
{
	if (Interface == nullptr)
		return;

	Interface->InGameMenuCancel();
}
