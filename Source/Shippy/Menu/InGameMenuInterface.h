// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Interface for InGameMenu to pass messages to it's owner.
 */
class SHIPPY_API InGameMenuInterface
{
public:

	virtual void InGameMenuExitToMainMenu() = 0;
	virtual void InGameMenuCancel() = 0;
};
