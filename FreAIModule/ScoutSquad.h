#pragma once

#include <BWAPI.h>

#include <set>
#include <map>

#include "Common.h"
#include "UnitSquad.h"
#include "ScoutManager.h"

class ScoutSquad
	: public UnitSquad
{
public:
	ScoutSquad(TimeVal updatePeriod = timeManager->fromMs(DEFAULT_SQUAD_UPDATE_PERIOD));

	void update();
	virtual void scout();

	virtual void displayInfo();

	virtual BWAPI::Position getTarget(UnitAgent* who = 0);

private:
	std::set<std::pair<double, ScoutInfo*> > _targets;
	std::map<UnitAgent*, ScoutInfo*> _targetMap;
	std::map<ScoutInfo*, UnitAgent*> _agentMap;
};
