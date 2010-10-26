#pragma once

#include <BWAPI.h>

#include <set>
#include <map>

#include "Common.h"
#include "UnitSquad.h"

#define DEFAULT_SQUAD_UPDATE_PERIOD 500

class AttackSquad
	: public UnitSquad
{
public:
	AttackSquad(TimeVal updatePeriod = timeManager->fromMs(DEFAULT_SQUAD_UPDATE_PERIOD));

	void update();
	virtual void attack();

	virtual void displayInfo();
};
