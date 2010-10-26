#include "BehaviorAgent.h"

BehaviorAgent::BehaviorAgent(UnitInfo *unit, TimeVal defaultUpdatePeriod, BehaviorManager::BehaviorType type) : UnitAgent(unit, defaultUpdatePeriod),
	_behavior(*(behaviorManager->getBehavior(behaviorManager->getBehaviorId(type))))
{
}