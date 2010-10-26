#pragma once

#include <windows.h>

#include <vector>

class Behavior;

class BehaviorManager
{
public:
	enum BehaviorType
	{
		InvalidBehavior,
		DefaultBehavior,
		SmartBehavior,
		WorkerBehavior,
		SuicidalBehavior,
		LastBehavior
	};

	void onStart();
	void registerBehavior(Behavior* behavior);
	Behavior* getBehavior(unsigned int behaviorId);
	unsigned int getBehaviorId(BehaviorManager::BehaviorType);

private:
	std::vector<unsigned int> _staticBehaviors;
	std::vector<Behavior*> _dynamicBehaviors;
};
