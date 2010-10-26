#include "UnitSquad.h"

#include "ScoutManager.h"
#include "OverlayManager.h"

using namespace BWAPI;

UnitSquad::UnitSquad(Order order, BWAPI::Position target, double targetRadius, TimeVal updatePeriod)
  : TimeLimiter(updatePeriod),
		_order(order),
	  _target(target),
		_targetRadius(targetRadius)
{
	informationManager->registerSquad(this);
}

UnitSquad::~UnitSquad()
{
	for (UnitAgentSet::iterator it = _units.begin(); it != _units.end(); ++it)
	{
		(*it)->setSquad(0);
	}
	informationManager->unregisterSquad(this);
}

void
UnitSquad::addUnit(UnitAgent* unit)
{
	UnitSquad* squad = unit->getSquad();
	if (squad)
		squad->removeUnit(unit);
	unit->setSquad(this);
	_units.insert(unit);
}

void
UnitSquad::removeUnit(UnitAgent* unit)
{
	UnitSquad* squad = unit->getSquad();
	if (squad != this)
		return;
	_units.erase(unit);
	unit->setSquad(0);
}

void
UnitSquad::displayInfo()
{
	Broodwar->drawCircle(CoordinateType::Map, _center.x(), _center.y(), (int) _spread, Colors::White, false);
	Broodwar->drawCircle(CoordinateType::Map, _center.x(), _center.y(), 5, Colors::White, true);
	if (_order == Move && _target != BWAPI::Positions::None)
	{
		Broodwar->drawCircle(CoordinateType::Map, _target.x(), _target.y(), (int) _targetRadius, Colors::Blue, false);
		Broodwar->drawCircle(CoordinateType::Map, _target.x(), _target.y(), 5, Colors::Blue, true);
		Broodwar->drawLine(CoordinateType::Map, _center.x(), _center.y(), _target.x(), _target.y(), Colors::Blue);
	}
	for (UnitAgentSet::iterator it = _units.begin(); it != _units.end(); ++it)
	{
		BWAPI::Position p = (*it)->getUnitInfo()->lastPosition;
		Broodwar->drawCircle(CoordinateType::Map, p.x() - 6, p.y() - 6, 2, Colors::White, true);
	}
}

void
UnitSquad::frameUpdate()
{
#ifdef DEBUG_DISPLAY
	if (overlayManager->overlay(OverlayManager::SquadOverlay))
	{
		displayInfo();
	}
#endif
}

void
UnitSquad::update()
{
	computeStatistics();
}

void
UnitSquad::computeStatistics()
{
	_center = BWAPI::Position(0, 0);
	for (UnitAgentSet::iterator it = _units.begin(); it != _units.end(); ++it)
	{
		_center += (*it)->getUnitInfo()->lastPosition;
	}
	if (_units.size() > 0)
		_center = BWAPI::Position(_center.x() / _units.size(), _center.y() / _units.size());
	_spread = 0;
	for (UnitAgentSet::iterator it = _units.begin(); it != _units.end(); ++it)
	{
		_spread += _center.getDistance((*it)->getUnitInfo()->lastPosition);
	}
	if (_units.size() > 0)
		_spread /= _units.size();
}

BWAPI::Position
UnitSquad::getTarget(UnitAgent*)
{
	return _target;
}
