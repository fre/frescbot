#include "BehaviorManager.h"

#include "Behavior.h"
#include "Behaviors.h"

void
BehaviorManager::onStart()
{
	std::map<const char*, double>* map; // NOTE: Never deleted.
	new Behavior(0); // InvalidBehavior
	new Behavior(BehaviorManager::getBehaviorId(BehaviorManager::InvalidBehavior)); // DefaultBehavior
	map = new std::map<const char*, double>;
	// Profile update periods
	(*map)["defaultDefaultPeriod"] = 1000;
	(*map)["defaultIdlePeriod"] = 1000;
	(*map)["defaultFindTargetPeriod"] = 100;
	(*map)["defaultFightPeriod"] = 50;
	(*map)["defaultFleePeriod"] = 200;
	(*map)["defaultFollowSquadPeriod"] = 1000;
	(*map)["defaultUserControlPeriod"] = 1000;
	// Base target choice score.
	(*map)["baseScore"] = 5000;
	(*map)["weightOptimalRange"] = -1; // Penalize away from optimal range [0; 1].
	(*map)["weightOutOfRange"] = -400; // Penalize out of range [0; inf * maxRange] = out of range %.
	(*map)["weightDamageFactor"] = 10; // Prioritize target with the right armor type [0; 1].
	(*map)["weightTargetHealth"] = -50; // Prioritize hurt targets [0; 1] = current health %.
	(*map)["weightThreatLevel"] = 1; // Prioritize high threat targets [0; xxx] = a * price + b * damage.
	(*map)["weightOverkill"] = -1; // Prioritize targets for precise kill [0; xxx] = abs(current health - (sum of attacks + ours)).
	// Behavior variables:
	(*map)["reevaluateTargetPeriod"] = 500;
	//
	(*map)["rangeOptimize"] = 1;
	(*map)["rangeOptimizeMinimumTime"] = 500;
	//
	(*map)["threatRadius"] = 500;
	(*map)["threatDistanceOffset"] = 10;
	(*map)["forceRadius"] = 500;
	(*map)["forceDistanceOffset"] = 10;
	(*map)["targetRadius"] = 500;
	//
	(*map)["threatLevelToIdle"] = 0; // or <.
	(*map)["threatLevelToFight"] = 1; // or >.
	(*map)["forceRatioToFlee"] = 0.5; // or >.
	// Hurt speed computation & management:
	(*map)["hurtSpeedToFlee"] = 0.30; // in % current life/s + 0.10 % max life/s, or >.
	(*map)["hurtSpeedSamples"] = 5;
	(*map)["hurtSpeedPeriod"] = 100;
	new Behavior(BehaviorManager::getBehaviorId(BehaviorManager::DefaultBehavior), map); // SmartBehavior
	new Behavior(BehaviorManager::getBehaviorId(BehaviorManager::SmartBehavior)); // WorkerBehavior
	new Behavior(BehaviorManager::getBehaviorId(BehaviorManager::SmartBehavior),
							 behaviors::suicidalBehaviorMap()); // SuicidalBehavior for i.e. scourge.
}

void
BehaviorManager::registerBehavior(Behavior* behavior)
{
	_dynamicBehaviors.push_back(behavior);
	behavior->setId(BehaviorManager::_dynamicBehaviors.size() - 1);
	if (_staticBehaviors.size() < BehaviorManager::LastBehavior)
		_staticBehaviors.push_back(behavior->getId());
}

Behavior*
BehaviorManager::getBehavior(unsigned int behaviorId)
{
	if (behaviorId == getBehaviorId(InvalidBehavior) || behaviorId >= _dynamicBehaviors.size())
		return (0);
	return (_dynamicBehaviors.at(behaviorId));
}

unsigned int
BehaviorManager::getBehaviorId(BehaviorManager::BehaviorType type)
{
	if (_staticBehaviors.size() > static_cast<unsigned int>(type))
		return (_staticBehaviors.at(type));
	return (0);
}
