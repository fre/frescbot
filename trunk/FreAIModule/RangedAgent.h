#pragma once

#include <BWAPI.h>

#include "Common.h"

#include "UnitInfo.h"
#include "AttackInfo.h"
#include "UnitAgent.h"

class RangedAgent
	: public UnitAgent
{
public:
	RangedAgent(UnitInfo* unit, TimeVal updatePeriod = timeManager->fromMs(DEFAULT_AGENT_UPDATE_PERIOD));

	// Look for and attack targets at range.
	virtual void update();
	virtual void displayDebugInfo();
	virtual void reaquire();
	virtual void attack();
	virtual void roam();

	virtual double computeTargetScore(UnitInfo* target);

protected:
	TimeVal _timeReaquirePeriod;
	TimeVal _timeReaquireNext;
	TimeVal _timeCooldown;
	TimeVal _defaultUpdatePeriod;

	UnitInfo* _target;
	UnitInfo* _lastTarget;	
};
