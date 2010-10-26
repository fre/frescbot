#include "AttackSquad.h"

#include "ScoutManager.h"

using namespace BWAPI;

#define ATTACK_SQUAD_TARGET_RADIUS 300

#define ATTACK_SQUAD_MAX_SPREAD_BASE 60
#define ATTACK_SQUAD_MAX_SPREAD_PER_UNIT 2

AttackSquad::AttackSquad(TimeVal updatePeriod)
  : UnitSquad(None, BWAPI::Positions::None, ATTACK_SQUAD_TARGET_RADIUS, updatePeriod)
{
}

void
AttackSquad::update()
{
	DEBUG("Attack squad " << this << " update.");
	UnitSquad::update();
	attack();
}

void
AttackSquad::attack()
{
	std::set<std::pair<double, ScoutInfo*> > target = scoutManager->getScoutTargets(1);
	if (target.size() > 0)
		setTarget(target.begin()->second->position);
	else
		setTarget(Position(Broodwar->mapWidth() / 2, Broodwar->mapHeight() / 2));
	if (_spread > ATTACK_SQUAD_MAX_SPREAD_BASE + ATTACK_SQUAD_MAX_SPREAD_PER_UNIT * _units.size())
		setOrder(Tighten);
	else
		setOrder(Move);
}

void
AttackSquad::displayInfo()
{
	UnitSquad::displayInfo();

	double maxSpread = ATTACK_SQUAD_MAX_SPREAD_BASE + ATTACK_SQUAD_MAX_SPREAD_PER_UNIT * _units.size();
	Broodwar->drawCircle(CoordinateType::Map, _center.x(), _center.y(), (int) maxSpread, Colors::Cyan, false);
}
