#include "InformationManager.h"

#include "OverlayManager.h"
#include "ScoutSquad.h"
#include "AttackSquad.h"
#include "SmartAgent.h"
#include "RangedAgent.h"
#include "WorkerAgent.h"

using namespace BWAPI;

#define DEFAULT_UNIT_AGENT SmartAgent
#define DEFAULT_INFORMATION_UPDATE_PERIOD 0

#define FLYER_THREAT_RATIO 1.2
#define BUILDING_THREAT_RATIO 0 // FIXME: Handle bunkers, cannons
#define BUILDING_TACTICAL_RATIO 0.8
#define WORKER_THREAT_RATIO 0.3
#define WORKER_TACTICAL_RATIO 2.5
#define DAMAGE_THREAT_RATIO 2

InformationManager::InformationManager()
  : TimeLimiter(timeManager->fromMs(DEFAULT_INFORMATION_UPDATE_PERIOD))
{
}

void
InformationManager::onStart()
{
	new AttackSquad();
	// Take into account initial units.
	const std::set<Unit*>& units = Broodwar->getAllUnits();
	for(std::set<Unit*>::const_iterator it = units.begin();
			it != units.end(); it++)
  {
		onUnitCreate(*it);
	}
}

void
InformationManager::displayUnitInfo(UnitInfo* unit)
{
	// Hit points
	if (!unit->visible())
		return;
	double maxHealth = unit->lastType.maxHitPoints() + unit->lastType.maxShields();
	double health = unit->unit->getHitPoints() + unit->unit->getShields();

	Broodwar->drawBox(CoordinateType::Map, unit->lastPosition.x() - 7, unit->lastPosition.y() + 7,
		                unit->lastPosition.x() - 7 + (int) (15 * (EPSILON + health) / (EPSILON + maxHealth)),
										unit->lastPosition.y() + 10,
										Colors::Green, true);
}

void
InformationManager::frameUpdate()
{
#ifdef DEBUG_DISPLAY
	if (overlayManager->overlay(OverlayManager::UnitInfoOverlay))
	{
		for (UnitInfoSet::iterator it = _all.begin();
				 it != _all.end(); ++it)
		{
			displayUnitInfo(*it);
		}
	}
#endif
}

void
InformationManager::update()
{
	DEBUG("InformationManager update.");
	TimeVal now = updateTime();
	for (UnitInfoSet::iterator it = _all.begin();
			 it != _all.end(); ++it)
	{
		UnitInfo* info = *it;
		Unit* unit = info->unit;

		if (unit->exists())
		{
			// Update position.
			info->lastTime = now;
			info->lastPosition = unit->getPosition();
		}
		else
		{
			// Workaround scourge bug:
			info->flags &= ~(UnitInfo::Visible);
			// Work around explosions:
			if (info->flags & UnitInfo::Self)
				onUnitDestroy(unit);
			// Mark units assumed dead.
			if (((info->flags & UnitInfo::Building) && now - info->lastTime > timeManager->fromMs(BUILDING_ASSUME_DEAD_PERIOD)) ||
				  ((info->flags & UnitInfo::Unit) && now - info->lastTime > timeManager->fromMs(UNIT_ASSUME_DEAD_PERIOD)))
			{
				onUnitDestroy(unit);
			}
		}
	}
}

static void
updateTypeFlags(UnitInfo* info, UnitType type)
{
	if (type.isFlyer())
		info->flags |= UnitInfo::Air;
	else
		info->flags |= UnitInfo::Ground;

	if (type.isBuilding())
		info->flags |= UnitInfo::Building;
	else
		info->flags |= UnitInfo::Unit;

	if (type.isWorker())
		info->flags |= UnitInfo::Worker;

	if (type.getID() == 176) // Mineral field
		info->flags |= UnitInfo::Mineral;
}

static void
updateValues(UnitInfo* info, UnitType type)
{
	// FIXME: get good values.
	info->threatValue = type.mineralPrice() + type.gasPrice();
	info->tacticalValue = type.mineralPrice() + type.gasPrice();
	if (type.isFlyer())
		info->threatValue *= FLYER_THREAT_RATIO;

	if (type.isBuilding())
	{
		info->threatValue *= BUILDING_THREAT_RATIO;
		info->tacticalValue *= BUILDING_TACTICAL_RATIO;
	}

	if (type.isWorker())
	{
		info->threatValue *= WORKER_THREAT_RATIO;
		info->tacticalValue *= WORKER_TACTICAL_RATIO;
	}

  const WeaponType* weapon = (info->flags & UnitInfo::Air) ? (type.airWeapon())
																													 : (type.groundWeapon());

	if (*weapon != WeaponTypes::None)
	{
		// FIXME: Maybe take attack type into account?
		// FIXME: Use DPS instead?
		double damage = max(1, (int) weapon->damageAmount());
		info->threatValue += damage * DAMAGE_THREAT_RATIO;
	}
}

void
InformationManager::onUnitCreate(BWAPI::Unit* unit)
{
	UnitInfo* info = new UnitInfo();

	info->id = unit->getID();
	info->unit = unit;
	info->flags = UnitInfo::None;
	info->flags |= UnitInfo::Exists;
	if (unit->isVisible())
		info->flags |= UnitInfo::Visible;

	info->lastPosition = unit->getPosition();
	info->lastTime = timeManager->now();
	info->lastType = unit->getType();

	updateTypeFlags(info, unit->getType());
	updateValues(info, unit->getType());

	_map[unit] = info;
	_all.insert(info);
	if (unit->getPlayer() == Broodwar->self())
	{
		info->flags |= UnitInfo::Self;
		_self.insert(info);
		// Create an agent for our own units
		if (info->flags & UnitInfo::Unit)
		{
			if (info->flags & UnitInfo::Worker)
			{
				_agents[unit] = new WorkerAgent(info);
			}
			else
			{
				switch (info->lastType.getID())
				{
				case 35: // Zerg Larva
				case 36: // Zerg Egg
					break;
				case 47: // Zerg Scourge
					_agents[unit] = new SmartAgent(info, 6, BehaviorManager::SuicidalBehavior);
					if (_squads.size() > 0)
						(*_squads.begin())->addUnit(_agents[unit]);
					break;
				default:
					// FIXME: Create the proper agent.
					_agents[unit] = new DEFAULT_UNIT_AGENT(info);
					if (_squads.size() > 0)
						(*_squads.begin())->addUnit(_agents[unit]);
				}
			}
	  }
	}
	else
	{
		if (Broodwar->self()->isEnemy(unit->getPlayer()))
		{
			info->flags |= UnitInfo::Enemy;
			_enemy.insert(info);
		}
		else
		{
			info->flags |= UnitInfo::Neutral;
			_neutral.insert(info);
		}
	}
}

void
InformationManager::onUnitDestroy(BWAPI::Unit* unit)
{
	// Do not remove unit info from map.

	UnitInfo* info = _map[unit];
	// Reset visible and exists flags.
	info->flags &= ~(UnitInfo::Visible | UnitInfo::Exists);
	info->lastPosition = unit->getPosition();
	info->lastTime = timeManager->now();

	_all.erase(info);
	if (info->flags & UnitInfo::Self)
	{
		if (info->flags & UnitInfo::Unit)
		{
			delete _agents[unit];
			_agents.erase(unit);
		}
		_self.erase(info);
	}
	else if (info->flags & UnitInfo::Enemy)
	{
		_enemy.erase(info);
	}
	else if (info->flags | UnitInfo::Neutral)
	{
		_neutral.erase(info);
	}
}

void
InformationManager::onUnitMorph(BWAPI::Unit* unit)
{
	// FIXME: Do something better?
	onUnitDestroy(unit);
	onUnitCreate(unit);
}

void
InformationManager::onUnitShow(BWAPI::Unit* unit)
{
	if (_map.find(unit) == _map.end())
		onUnitCreate(unit);
	UnitInfo* info = _map[unit];
	if (!(info->flags & UnitInfo::Exists))
	{
		// Unit assumed dead, put back in sets.
		_all.insert(info);
		if (info->flags & UnitInfo::Enemy)
			_enemy.insert(info);
		else if (info->flags & UnitInfo::Neutral)
			_neutral.insert(info);
	}
	info->flags |= (UnitInfo::Visible | UnitInfo::Exists);
	info->lastType = unit->getType();
	info->lastPosition = unit->getPosition();
	info->lastTime = timeManager->now();
}

void
InformationManager::onUnitHide(BWAPI::Unit* unit)
{
	UnitInfo* info = _map[unit];
	info->flags &= ~(UnitInfo::Visible);
	info->lastPosition = unit->getPosition();
	info->lastTime = timeManager->now();
}

void
InformationManager::onUnitRenegade(BWAPI::Unit* unit)
{
	// FIXME.
}


static UnitInfoSet
extractSet(const UnitInfoSet& set, int flags)
{
	UnitInfoSet result;
	for (UnitInfoSet::const_iterator it = set.begin(); it != set.end(); ++it)
	{
		if (((*it)->flags & flags) == flags)
			result.insert(*it);
	}
	return result;
}

UnitInfoSet
InformationManager::all(int flags)
{
	return extractSet(_all, flags);
}

UnitInfoSet
InformationManager::neutral(int flags)
{
	return extractSet(_neutral, flags);
}

UnitInfoSet
InformationManager::self(int flags)
{
	return extractSet(_self, flags);
}

UnitInfoSet
InformationManager::enemy(int flags)
{
	return extractSet(_enemy, flags);
}

static UnitInfoSet
extractClosestSet(const UnitInfoSet& set, int n, BWAPI::Position position, int flags)
{
	std::set<std::pair<double, UnitInfo*> > neighbors;
	for (UnitInfoSet::const_iterator it = set.begin(); it != set.end(); ++it)
	{
		if (((*it)->flags & flags) == flags)
		{
			neighbors.insert(std::make_pair(position.getDistance((*it)->lastPosition), *it));
		}
	}
	UnitInfoSet result;
	for (std::set<std::pair<double, UnitInfo*> >::iterator it = neighbors.begin();
			 it != neighbors.end() && n > 0; ++it, --n)
	{
		result.insert(it->second);
	}
	return result;
}

UnitInfoSet
InformationManager::closestAll(int n, BWAPI::Position position, int flags)
{
	return extractClosestSet(_all, n, position, flags);
}

UnitInfoSet
InformationManager::closestNeutral(int n, BWAPI::Position position, int flags)
{
	return extractClosestSet(_neutral, n, position, flags);
}

UnitInfoSet
InformationManager::closestSelf(int n, BWAPI::Position position, int flags)
{
	return extractClosestSet(_self, n, position, flags);
}

UnitInfoSet
InformationManager::closestEnemy(int n, BWAPI::Position position, int flags)
{
	return extractClosestSet(_enemy, n, position, flags);
}

static UnitInfoSet
extractCloseSet(const UnitInfoSet& set, BWAPI::Position position, double radius, int flags)
{
	UnitInfoSet result;
	for (UnitInfoSet::const_iterator it = set.begin(); it != set.end(); ++it)
	{
		if (((*it)->flags & flags) == flags && position.getDistance((*it)->lastPosition) <= radius)
		{
			result.insert(*it);
		}
	}
	return result;
}

UnitInfoSet
InformationManager::closeAll(BWAPI::Position position, double radius, int flags)
{
	return extractCloseSet(_all, position, radius, flags);
}

UnitInfoSet
InformationManager::closeNeutral(BWAPI::Position position, double radius, int flags)
{
	return extractCloseSet(_neutral, position, radius, flags);
}

UnitInfoSet
InformationManager::closeSelf(BWAPI::Position position, double radius, int flags)
{
	return extractCloseSet(_self, position, radius, flags);
}

UnitInfoSet
InformationManager::closeEnemy(BWAPI::Position position, double radius, int flags)
{
	return extractCloseSet(_enemy, position, radius, flags);
}

static double
damageFactor(const WeaponType* weapon, const UnitInfo* target)
{
	//               Size:     Ind   Sma   Med   Lar   Non   Unk
	static double values[] = { 1.00, 1.00, 0.50, 0.25, 1.00, 1.00, // Concussive
														 1.00, 0.50, 0.75, 1.00, 1.00, 1.00, // Explosive
														 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, // Ignore_Armor
														 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, // Independent
														 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, // Normal
														 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, // None
														 1.00, 1.00, 1.00, 1.00, 1.00, 1.00 }; // Unknown

	// FIXME: Maybe adjust for partial shields?
	if (target->unit->getShields() > 0)
		return 1;

	return values[target->lastType.size().getID() + weapon->damageType()->getID() * 6];
}

AttackInfo
InformationManager::computeAttack(UnitInfo* unit, UnitInfo* target)
{
	AttackInfo attack;

	attack.possible = false;
	if (!(unit && unit->exists() && target && target->exists()))
		return attack;
	// FIXME: decide what to do for invisible units.
	if (!target->visible())
		return attack;
	if (target->lastType.isInvincible())
		return attack;

	const WeaponType* weapon = (target->flags & UnitInfo::Air) ? (unit->lastType.airWeapon())
																														 : (unit->lastType.groundWeapon());

	if (*weapon == WeaponTypes::None)
		return attack;

	// Cloaked unit -> attack impossible.
	//if (!target->visible() && Broodwar->isVisible(target->lastPosition))
	//	return attack;

	attack.possible = true;
	attack.currentRange = (int) unit->lastPosition.getDistance(target->lastPosition);
	attack.minRange = weapon->minRange();
	attack.maxRange = weapon->maxRange();
	attack.optimalRange = attack.minRange + attack.maxRange / 2;
	attack.inRange = attack.currentRange > attack.minRange && attack.currentRange < attack.maxRange;
	attack.cooldown = weapon->damageCooldown();
	attack.damageFactor = damageFactor(weapon, target);
  // FIXME: take upgrades and shields into account.
	attack.damage = max(1, (int) ((weapon->damageAmount() - target->lastType.armor()) * attack.damageFactor));
	return attack;
}
