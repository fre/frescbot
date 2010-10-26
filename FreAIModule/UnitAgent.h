#pragma once

#include <windows.h>

#include <BWAPI.h>

#include "Common.h"

#include "TimeManager.h"
#include "TimeLimiter.h"
#include "UnitInfo.h"

// In ms.
#define DEFAULT_AGENT_UPDATE_PERIOD 1000

class UnitSquad;

class UnitAgent
	: public TimeLimiter
{
public:
	UnitAgent(UnitInfo* unit, TimeVal updatePeriod = timeManager->fromMs(DEFAULT_AGENT_UPDATE_PERIOD));
	virtual ~UnitAgent();

	// Update agent (forced, overload this).
	virtual void update();
	virtual void frameUpdate();

	// Display debug info, e.g. attack lines.
	virtual void displayDebugInfo();

	// Unit info method:
	inline UnitInfo* getUnitInfo() { return _unit; }

	// Squad methods:
	inline UnitSquad* getSquad() { return _squad; }
	// Only call from UnitSquad::AddUnit().
	inline void setSquad(UnitSquad* squad) { _squad = squad; }

protected:

	UnitInfo* _unit;
	UnitSquad* _squad;
};
