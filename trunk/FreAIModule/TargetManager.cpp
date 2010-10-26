#include "TargetManager.h"

#include "OverlayManager.h"

using namespace BWAPI;

TargetManager::TargetManager()
  : TimeLimiter(timeManager->fromMs(100))
{
}

void
TargetManager::onStart()
{
}

void
TargetManager::frameUpdate()
{
#ifdef DEBUG_DISPLAY
	displayInfo();
#endif
}

void
TargetManager::update()
{
	// Remove targets with no damage, update life totals.
	for (std::map<UnitInfo*, TargetInfo>::iterator it = _targets.begin();
			 it != _targets.end(); )
	{
		if (it->second.damage == 0)
		{
			std::map<UnitInfo*, TargetInfo>::iterator t = it;
			++it;
			_targets.erase(t);
			continue;
		}
		BWAPI::Unit* unit = it->first->unit;
		it->second.health = unit->getHitPoints() + unit->getShields();
		++it;
	}
}

void
TargetManager::displayInfo()
{
}

void
TargetManager::addDamage(UnitInfo* target, double damage)
{
	if (!target)
		return;
	if (_targets.find(target) == _targets.end())
	{
		double health = target->unit->getHitPoints() + target->unit->getShields();
		_targets[target] = TargetInfo(health);
	}
	_targets[target].damage += damage;
}

void
TargetManager::removeDamage(UnitInfo* target, double damage)
{
	if (!target || _targets.find(target) == _targets.end())
		return;
	double targetDamage = _targets[target].damage - damage;
  _targets[target].damage = targetDamage;
}

const TargetInfo&
TargetManager::targetInfo(UnitInfo* target)
{
	if (_targets.find(target) == _targets.end())
	{
		double health = target->unit->getHitPoints() + target->unit->getShields();
		_targets[target] = TargetInfo(health);
	}
	return _targets[target];
}

TargetInfo::TargetInfo(double l, double d)
  : health(l),
    damage(d)
{
}