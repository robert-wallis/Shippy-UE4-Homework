// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Interface for a ServerRow widget.
 */
class SHIPPY_API ServerRowInterface
{
public:

	virtual void ServerRowJoin(const FString &Address) = 0;

};
