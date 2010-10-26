#pragma once

#include <windows.h>

#include <BWAPI.h>

#include "Common.h"

#include "TimeManager.h"
#include "TimeLimiter.h"

// Organize squads.
class TacticalManager
	: public TimeLimiter
{
public:
	// Information update.
	void onStart();

	void update();
	void frameUpdate();

	void displayInfo();
};
