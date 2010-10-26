#pragma once

#include <windows.h>

#include <BWAPI.h>

#include <map>

#include "Common.h"

#include "TimeManager.h"
#include "TimeLimiter.h"
#include "UnitInfo.h"

struct TargetInfo
{
	TargetInfo(double health = 0, double damage = 0);

	double damage;
	double health;
};

// Organize squads.
class TargetManager
	: public TimeLimiter
{
public:
	TargetManager();

	// Information update.
	void onStart();

	void update();
	void frameUpdate();

	void displayInfo();

	const TargetInfo& targetInfo(UnitInfo* target);
	void addDamage(UnitInfo* target, double damage);
	void removeDamage(UnitInfo* target, double damage);

private:
	std::map<UnitInfo*, TargetInfo> _targets;
};
