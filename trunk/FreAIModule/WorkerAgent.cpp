#include "WorkerAgent.h"

#include "UnitSquad.h"

using namespace BWAPI;

// FIXME: Add behaviors.

// Profile update periods
#define DEFAULT_GET_MINERALS_PERIOD 1000
#define DEFAULT_GET_GAS_PERIOD 1000
#define DEFAULT_FOLLOW_BASE_PERIOD 1000

#define FORCE_RATIO_TO_FIGHT 0.5

WorkerAgent::WorkerAgent(UnitInfo* unit, int customProfiles, BehaviorManager::BehaviorType type)
  : SmartAgent(unit, customProfiles, type),
		_base(0)
{
	setProfileUpdatePeriod(GetMinerals, timeManager->fromMs(DEFAULT_GET_MINERALS_PERIOD));
	setProfileUpdatePeriod(GetGas, timeManager->fromMs(DEFAULT_GET_GAS_PERIOD));
	setProfileUpdatePeriod(FollowBase, timeManager->fromMs(DEFAULT_FOLLOW_BASE_PERIOD));
}

void
WorkerAgent::profileUpdate(int profile)
{
	// Handle first 7 profiles + assessments.
	SmartAgent::profileUpdate(profile);
	DEBUG("WorkerAgent " << _unit->id << ":");

	switch (getProfile())
	{
	case GetMinerals:
		getMinerals();
		break;
	case GetGas:
		getGas();
		break;
	case FollowBase:
		followBase();
		break;
	default:
		return;
	}
}

// Display debug info, e.g. attack lines.
void
WorkerAgent::displayDebugInfo()
{
	SmartAgent::displayDebugInfo();
#ifdef DEBUG_DISPLAY
	int r = _unit->lastType.dimensionLeft();
	int x = _unit->lastPosition.x();
	int y = _unit->lastPosition.y();
	int tx = _target ? _target->lastPosition.x() : x;
	int ty = _target ? _target->lastPosition.y() : y;

	switch (getProfile())
	{
	case FollowBase:
		Broodwar->drawCircle(CoordinateType::Map, x, y, r, Colors::Yellow, false);
		break;
	case GetMinerals:
		Broodwar->drawCircle(CoordinateType::Map, x, y, r, Colors::Cyan, false);
		break;
	case GetGas:
		Broodwar->drawCircle(CoordinateType::Map, x, y, r, Color(150, 150, 0), false);
		break;
	default:
		return;
	}
#endif
}

// Profile functions:
void
WorkerAgent::flee()
{
	if (forceRatio() > FORCE_RATIO_TO_FIGHT)
	{
		setProfile(Idle);
		return;
	}
	SmartAgent::flee();
}

void
WorkerAgent::findTarget()
{
	if (forceRatio() > FORCE_RATIO_TO_FIGHT)
	{
		setProfile(Idle);
		return;
	}
	SmartAgent::findTarget();
	if (_target == 0)
		setProfile(GetMinerals); // FIXME: GetGas?
}

void
WorkerAgent::fight()
{
	if (forceRatio() > FORCE_RATIO_TO_FIGHT)
	{
		setProfile(Idle);
		return;
	}
	SmartAgent::fight();
}

void
WorkerAgent::idle()
{
	if (_base)
	{
		setProfile(FollowBase);
	}
	if (forceRatio() <= FORCE_RATIO_TO_FIGHT)
		setProfile(FindTarget);
	// FIXME: GetGas?
	setProfile(GetMinerals);
}

void
WorkerAgent::followSquad()
{
	SmartAgent::followSquad();
}

void
WorkerAgent::getMinerals()
{
	if (forceRatio() <= FORCE_RATIO_TO_FIGHT)
		setProfile(FindTarget);
	if (_base)
	{
		// Get spot from base?
	}
	else
	{
		UnitInfoSet closestMineral = informationManager->closestNeutral(1, _unit->lastPosition, UnitInfo::Mineral);
		if (closestMineral.size() == 0)
		{
			DEBUG("Could not find mineral patch.");
		}
		else
		{
			Unit* unit = _unit->unit;
			Order order = unit->getOrder();
			// Do not click again if already gathering minerals.
			if (!(order == Orders::MoveToMinerals ||
				    order == Orders::WaitForMinerals ||
						order == Orders::MiningMinerals ||
						order == Orders::ReturnMinerals))
				unit->rightClick((*(closestMineral.begin()))->unit);
		}
	}
}

void
WorkerAgent::getGas()
{
	if (forceRatio() <= FORCE_RATIO_TO_FIGHT)
		setProfile(FindTarget);
	// FIXME: Get minerals.
	setProfile(GetMinerals);
}

void
WorkerAgent::followBase()
{
	if (forceRatio() <= FORCE_RATIO_TO_FIGHT)
		setProfile(FindTarget);
	getMinerals();
}

