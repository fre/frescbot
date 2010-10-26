#pragma once

#include <BWAPI.h>

#include "Common.h"

#include "SmartAgent.h"
#include "InformationManager.h"
#include "StandardBase.h"

// Worker agent, gather resources.
//
// Implementation: simple state machine.
// Be careful not to introduce infinite loops in the same frame.
//
class WorkerAgent
	: public SmartAgent
{
public:
	enum Profile
	{
		Default = 0,
		Idle = 1,
		FindTarget = 2,
		Fight = 3,
		Flee = 4,
		FollowSquad = 5,
		UserControl = 6,
		GetMinerals = 7,
		GetGas = 8,
		FollowBase = 9
	};

	WorkerAgent(UnitInfo* unit, int customProfiles = 9, BehaviorManager::BehaviorType type = BehaviorManager::WorkerBehavior);

	virtual void profileUpdate(int profile);

	// Display debug info, e.g. attack lines.
	virtual void displayDebugInfo();

	// Profile functions:
	virtual void flee();
	virtual void findTarget();
	virtual void fight();
	virtual void idle();
	virtual void followSquad();
	virtual void getMinerals();
	virtual void getGas();
	virtual void followBase();

	inline StandardBase* getBase() { return _base; }
	inline void setBase(StandardBase* base) { _base = base; };

protected:
	StandardBase* _base;
};
