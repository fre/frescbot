#pragma once

#include <vector>

#include <BWAPI.h>

#include "Common.h"

#include "UnitAgent.h"

#include "BehaviorManager.h"
#include "Behavior.h"

class BehaviorAgent : public UnitAgent
{
public:
	BehaviorAgent(UnitInfo* unit, TimeVal defaultUpdatePeriod, BehaviorManager::BehaviorType behaviorType);

protected:
	Behavior _behavior;
};
