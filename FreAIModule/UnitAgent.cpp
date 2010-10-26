#include "UnitAgent.h"

#include "UnitSquad.h"
#include "OverlayManager.h"

using namespace BWAPI;

UnitAgent::UnitAgent(UnitInfo *unit, TimeVal updatePeriod)
	: TimeLimiter(updatePeriod),
		_unit(unit),
		_squad(0)
{
}

UnitAgent::~UnitAgent()
{
	if (_squad)
		_squad->removeUnit(this);
	_squad = 0;
}

void
UnitAgent::frameUpdate()
{
#ifdef DEBUG_DISPLAY
	if (overlayManager->overlay(OverlayManager::AgentOverlay))
	{
		// Display debug info e.g. attack lines.
		displayDebugInfo();
	}
#endif
}

void
UnitAgent::update()
{
	DEBUG("Agent " << _unit->id << ":");
	DEBUG("** Idle **");
}

void
UnitAgent::displayDebugInfo()
{
}
