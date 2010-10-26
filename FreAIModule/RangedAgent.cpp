#include "RangedAgent.h"

#include "InformationManager.h"

using namespace BWAPI;

#define CLOSEST_TARGETS_COUNT 10

#define REAQUIRE_PERIOD_FACTOR 1
#define FIGHT_PERIOD_FACTOR 0.05
#define MINIMUM_ATTACK_PERIOD 300

#define RANGE_OPTIMIZE 1
#define RANGE_OPTIMIZE_MINIMUM_COOLDOWN 1000

// Base target choice score.
#define BASE_SCORE 500
#define WEIGHT_OPTIMAL_RANGE -1 // Penalize away from optimal range [0; 1].
#define WEIGHT_OUT_OF_RANGE -100 // Penalize out of range [0; inf * maxRange] = out of range %.
#define WEIGHT_DAMAGE_FACTOR 10 // Prioritize target with the right armor type [0; 1].
#define WEIGHT_TARGET_HEALTH -50 // Prioritize hurt targets [0; 1] = current health %.

RangedAgent::RangedAgent(UnitInfo* unit, TimeVal updatePeriod)
  : UnitAgent(unit, updatePeriod),
		_timeReaquirePeriod(updatePeriod * REAQUIRE_PERIOD_FACTOR),
		_defaultUpdatePeriod(updatePeriod),
		_target(0),
		_lastTarget(0)
{
	timeManager->now(_timeReaquireNext);
}

void
RangedAgent::update()
{
	DEBUG("Ranged Agent " << _unit->id << ":");

	// Get the best possible target.
	if ((_target && !_target->exists()) || updateTime() > _timeReaquireNext)
	{
#ifdef DEBUG_DISPLAY
		Broodwar->drawCircle(CoordinateType::Map, _unit->lastPosition.x(), _unit->lastPosition.y(), 15, Colors::Green, false);
#endif
		reaquire();
	}

	// Do not change orders during attack animation
	if (_unit->unit->isStartingAttack())
		return;

	// Attack current target.
	if (_target)
	{
		attack();
		return;
	}

	roam();
	return;
}

void
RangedAgent::reaquire()
{
	double bestScore = 0;
	UnitInfo* bestTarget = 0;

	UnitInfoSet targets(informationManager->closestEnemy(CLOSEST_TARGETS_COUNT, _unit->lastPosition));
	// Can probably be done in another thread.
	for (UnitInfoSet::iterator it = targets.begin();
			 it != targets.end(); ++it)
	{
		UnitInfo* target = *it;
		double score = computeTargetScore(target);
		DEBUG("  Seen " << target->lastType.getName() << ", score = " << score);
		if (score > bestScore)
		{
			bestScore = score;
			bestTarget = target;
		}
	}

	if (bestScore == 0)
	{
		DEBUG(" - No target acquired.");
		_target = 0;
		return;
	}
	else
	{
		_target = bestTarget;
		DEBUG(" - Aquired target " << _target->id << ".");
	}
	_timeReaquireNext = updateTime() + _defaultUpdatePeriod * REAQUIRE_PERIOD_FACTOR;
}

void
RangedAgent::roam()
{
	DEBUG(" - No action taken.");
}

void
RangedAgent::attack()
{
	AttackInfo attack = informationManager->computeAttack(_unit, _target);

	if (!attack.possible)
	{
		// Target is no longer available.
		_target = 0;
		_lastTarget = 0;
		_timeReaquireNext = updateTime();
		setUpdatePeriod(0);
		return;
	}

	if (_target != _lastTarget)
	{
		DEBUG(" - Attacking target " << _target->id << ".");
#ifdef DEBUG_DISPLAY
		Broodwar->drawCircle(CoordinateType::Map, _unit->lastPosition.x(), _unit->lastPosition.y(), 5, Colors::Red, true);
#endif
		_unit->unit->rightClick(_target->unit);
		_lastTarget = _target;
		_timeCooldown = updateTime() + timeManager->fromMs(attack.cooldown);
		setUpdatePeriod(max(timeManager->fromMs(MINIMUM_ATTACK_PERIOD), (TimeVal) (_defaultUpdatePeriod * FIGHT_PERIOD_FACTOR)));
		return;
	}
	else
	{
		if (RANGE_OPTIMIZE && attack.cooldown >= RANGE_OPTIMIZE_MINIMUM_COOLDOWN)
		{
			DEBUG(" - Moving to optimal range of target " << _target->id << ".");
			if (attack.currentRange > attack.optimalRange)
			{
				// Go towards the target
				_unit->unit->rightClick(_target->lastPosition);
				_lastTarget = 0;
#ifdef DEBUG_DISPLAY
				Broodwar->drawCircle(CoordinateType::Map, _unit->lastPosition.x(), _unit->lastPosition.y(), 5, Colors::Green, true);
#endif
			}
			else
			{
				// Go away from the target
				_unit->unit->rightClick(_unit->lastPosition - (_target->lastPosition - _unit->lastPosition));
				_lastTarget = 0;
#ifdef DEBUG_DISPLAY
				Broodwar->drawCircle(CoordinateType::Map, _unit->lastPosition.x(), _unit->lastPosition.y(), 5, Colors::Yellow, true);
#endif
			}
		}
		else
		{
			DEBUG(" - Continuing attack on " << _target->id << ".");
#ifdef DEBUG_DISPLAY
			Broodwar->drawCircle(CoordinateType::Map, _unit->lastPosition.x(), _unit->lastPosition.y(), 3, Colors::Blue, true);
#endif
		}
		setUpdatePeriod((TimeVal) (_defaultUpdatePeriod * FIGHT_PERIOD_FACTOR));
		return;
	}
}

double
RangedAgent::computeTargetScore(UnitInfo* target)
{
	AttackInfo attack = informationManager->computeAttack(_unit, target);

	// NOTE: invisible units are not included.
	if (!attack.possible)
		return 0;

	double score = BASE_SCORE;

	// Range weighting.
	if (attack.inRange)
		score += abs(attack.currentRange - attack.optimalRange) / (double) attack.maxRange * 2 * WEIGHT_OPTIMAL_RANGE;
	else
		// FIXME: What if too close?
		score += abs(attack.currentRange - attack.maxRange) / (double) attack.maxRange * WEIGHT_OUT_OF_RANGE;

	// FIXME: add threat level weighting.

	// FIXME: add damage weighting.
	score += attack.damageFactor * WEIGHT_DAMAGE_FACTOR;

	// Unit hurt weighting.
	score += (target->unit->getHitPoints() + target->unit->getShields())
				    / (double) (target->lastType.maxHitPoints() + target->lastType.maxShields()) * WEIGHT_TARGET_HEALTH;

	return score;
}


void
RangedAgent::displayDebugInfo()
{
	if (_target && _target->exists())
	{
		Broodwar->drawLine(CoordinateType::Map, _unit->lastPosition.x(), _unit->lastPosition.y(),
											 _target->lastPosition.x(), _target->lastPosition.y(), Colors::Red);
	}
}
