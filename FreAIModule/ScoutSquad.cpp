#include "ScoutSquad.h"

#include "ScoutManager.h"

#define SCOUT_SQUAD_TARGET_RADIUS 200

ScoutSquad::ScoutSquad(TimeVal updatePeriod)
  : UnitSquad(Move, BWAPI::Positions::None, SCOUT_SQUAD_TARGET_RADIUS, updatePeriod)
{
}

void
ScoutSquad::update()
{
	UnitSquad::update();
	scout();
}

void
ScoutSquad::scout()
{
	// FIXME: Handle flying units.
	_targets = scoutManager->getScoutTargets(max(1, _units.size()), true);
	_agentMap.clear();
	for (std::set<std::pair<double, ScoutInfo*> >::iterator it = _targets.begin(); it != _targets.end(); ++it)
	{
		_agentMap[it->second] = 0;
	}
	for (std::map<UnitAgent*, ScoutInfo*>::iterator it = _targetMap.begin(); it != _targetMap.end();)
	{
		if (_agentMap.count(it->second) > 0)
		{
			_agentMap[it->second] = it->first;
			++it;
		}
		else
		{
			std::map<UnitAgent*, ScoutInfo*>::iterator t = it;
			++it;
			_targetMap.erase(t);
		}
	}
}

BWAPI::Position
ScoutSquad::getTarget(UnitAgent* who)
{
	// Find the most important unassigned target.
	if (_targets.size() == 0)
		return BWAPI::Positions::None;
	if (!who)
		return _targets.rbegin()->second->position;
	if (_targetMap.count(who) > 0)
		return _targetMap[who]->position;
	for (std::set<std::pair<double, ScoutInfo*> >::reverse_iterator it = _targets.rbegin(); it != _targets.rend(); ++it)
	{
		if (_agentMap.count(it->second) > 0 && _agentMap[it->second] != 0)
			continue;
		// Request the target.
		_agentMap[it->second] = who;
		_targetMap[who] = it->second;
		return it->second->position;
	}
	return _targets.rbegin()->second->position;
}

void
ScoutSquad::displayInfo()
{
	// Not needed:
	// UnitSquad::displayInfo();

	for (std::map<ScoutInfo*, UnitAgent*>::iterator it = _agentMap.begin(); it != _agentMap.end(); ++it)
	{
		BWAPI::Position t = it->first->position;
		Broodwar->drawCircle(CoordinateType::Map, t.x(), t.y(), 7, Colors::Blue, true);
		Broodwar->drawCircle(CoordinateType::Map, t.x(), t.y(), 10, Colors::Blue, false);
		if (!it->second)
			continue;
		BWAPI::Position s = it->second->getUnitInfo()->lastPosition;
		Broodwar->drawLine(CoordinateType::Map, s.x(), s.y(), t.x(), t.y(), Colors::Blue);
	}
}
