#pragma once

#include <BWAPI.h>

#include "Common.h"

// Store information on a potential attack.
struct AttackInfo
{
	bool possible;
	bool inRange;

	int minRange;
	int maxRange;
	int optimalRange;
	int currentRange;

	int cooldown;
	double damageFactor; // How efficient is the attack
	int damage;
};