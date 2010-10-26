#include "SmartAgent.h"

#include "UnitSquad.h"
#include "TargetManager.h"

#define DEFAULT_DEFAULT_PERIOD 1000

using namespace BWAPI;

SmartAgent::SmartAgent(UnitInfo* unit, int customProfiles, BehaviorManager::BehaviorType type)
  : ProfileAgent(unit, customProfiles, timeManager->fromMs(DEFAULT_DEFAULT_PERIOD), type),
		_timeCooldown(0),
		_timeReevaluateTarget(0),
	  _target(0),
		_lastTarget(0),
		_lastDamage(0),
		_threatLevel(0),
		_threatVector(0, 0),
		_forceLevel(0),
		_forceVector(0, 0),
		_lastHealth(1),
		_timeHealth(0),
		_hurtSpeed(0),
		_hurtSpeedSample(0)
{
	_hurtSpeedPeriod = timeManager->fromMs(_behavior["hurtSpeedPeriod"]);
	setProfileUpdatePeriod(Default, timeManager->fromMs(_behavior["defaultDefaultPeriod"]));
	setProfileUpdatePeriod(Idle, timeManager->fromMs(_behavior["defaultIdlePeriod"]));
	setProfileUpdatePeriod(FindTarget, timeManager->fromMs(_behavior["defaultFindTargetPeriod"]));
	setProfileUpdatePeriod(Fight, timeManager->fromMs(_behavior["defaultFightPeriod"]));
	setProfileUpdatePeriod(Flee, timeManager->fromMs(_behavior["defaultFleePeriod"]));
	setProfileUpdatePeriod(FollowSquad, timeManager->fromMs(_behavior["defaultFollowSquadPeriod"]));
	setProfileUpdatePeriod(UserControl, timeManager->fromMs(_behavior["defaultUserControlPeriod"]));

	for (double i = _behavior["hurtSpeedSamples"]; i > 0; --i)
	{
		_hurtSpeedSamples.push_back(0);
	}
	timeManager->now(_timeHealth);
}

void
SmartAgent::profileUpdate(int)
{
	DEBUG("SmartAgent " << _unit->id << "(" << _unit->lastType.getName() << "):");

	// Threat assessment.
	assessHurtSpeed();
	assessThreatLevel();
	assessForceLevel();

	if (Broodwar->isFlagEnabled(Flag::UserInput) && _unit->unit->isSelected())
	{
		setProfile(UserControl);
		targetManager->removeDamage(_lastTarget, _lastDamage);
		_lastDamage = 0;
	}

	switch (getProfile())
	{
	case Default:
	case Idle:
		idle();
		break;
	case Fight:
		fight();
		break;
	case Flee:
		flee();
		break;
	case FollowSquad:
		followSquad();
		break;
	case FindTarget:
		findTarget();
		break;
	case UserControl:
		userControl();
		break;
	default:
		return;
	}
}

void
SmartAgent::assessHurtSpeed()
{
	if (updateTime() < _timeHealth)
		return;

	// Zero unmeasured samples.
	while (updateTime() >= _timeHealth)
	{
		_hurtSpeedSamples[_hurtSpeedSample] = 0;
		_hurtSpeedSample = (_hurtSpeedSample + 1) % _hurtSpeedSamples.size();
		_timeHealth += _hurtSpeedPeriod;
	}

	// Measure last sample
	double health = (double) (_unit->unit->getHitPoints() + _unit->unit->getShields())
		               / (_unit->lastType.maxHitPoints() + _unit->lastType.maxShields());
	_hurtSpeedSamples[(_hurtSpeedSample + _hurtSpeedSamples.size() - 1) % _hurtSpeedSamples.size()] = _lastHealth - health;
	_lastHealth = health;
	_hurtSpeed = 0;

	// Compute average
	for (unsigned i = 0; i < _hurtSpeedSamples.size(); ++i)
	{
		_hurtSpeed += _hurtSpeedSamples[i];
	}
	_hurtSpeed /= (timeManager->toS(_hurtSpeedPeriod) * _hurtSpeedSamples.size());
}

void
SmartAgent::assessThreatLevel()
{
	// FIXME: Use better flags.
	_threats = informationManager->closeEnemy(_unit->lastPosition, _behavior["threatRadius"], UnitInfo::Visible | UnitInfo::Unit); 
	_threatLevel = _threats.size();
	_threatVector = Vector2D(0, 0);
	for (UnitInfoSet::iterator it = _threats.begin(); it != _threats.end(); ++it)
	{
		Vector2D vector = ((*it)->lastPosition - _unit->lastPosition);
		double distance = vector.getLength();
		_threatVector += vector / (distance + _behavior["threatDistanceOffset"]);
	}
	if (!_threatVector.isNull())
		_threatVector /= _threatVector.getLength(); // Normalize.
}

void
SmartAgent::assessForceLevel()
{
	// FIXME: Use better flags.
	_forces = informationManager->closeSelf(_unit->lastPosition, _behavior["forceRadius"], UnitInfo::Visible | UnitInfo::Unit);
	_forceLevel = _forces.size();
	_forceVector = Vector2D(0, 0);
	for (UnitInfoSet::iterator it = _forces.begin(); it != _forces.end(); ++it)
	{
		Vector2D vector = ((*it)->lastPosition - _unit->lastPosition);
		double distance = vector.getLength();
		_forceVector += vector / (distance + _behavior["forceDistanceOffset"]);
	}
	if (!_forceVector.isNull())
		_forceVector /= _forceVector.getLength(); // Normalize.
}

// Display debug info, e.g. attack lines.
void
SmartAgent::displayDebugInfo()
{
#ifdef DEBUG_DISPLAY
	int r = _unit->lastType.dimensionLeft();
	int x = _unit->lastPosition.x();
	int y = _unit->lastPosition.y();
	int tx = _target ? _target->lastPosition.x() : x;
	int ty = _target ? _target->lastPosition.y() : y;

	// Threat & force display:
	{
		Vector2D v;
		v = (_threatVector * 30) + _unit->lastPosition;
		Broodwar->drawLine(CoordinateType::Map, x, y,
			                 (int) v.x, (int) v.y, Colors::Orange);
		Broodwar->drawCircle(CoordinateType::Map, (int) v.x, (int) v.y,
			                   max(1, min(10, (int) (_threatLevel / 2))), Colors::Orange, false);
	}
	{
		Vector2D v;
		v = (_forceVector * 30) + _unit->lastPosition;
		Broodwar->drawLine(CoordinateType::Map, x, y,
			                 (int) v.x, (int) v.y, Colors::Blue);
		Broodwar->drawCircle(CoordinateType::Map, (int) v.x, (int) v.y,
			                   max(1, min(10, (int) (_forceLevel / 2))), Colors::Blue, false);
	}
	switch (getProfile())
	{
	case Default:
	case Idle:
		Broodwar->drawCircle(CoordinateType::Map, x, y, r, Colors::White, false);
		break;
	case Fight:
		Broodwar->drawCircle(CoordinateType::Map, x, y, r, Colors::Red, false);
		if (_target && _target->exists())
			Broodwar->drawLine(CoordinateType::Map, x, y, tx, ty, Colors::Red);
		break;
	case Flee:
		Broodwar->drawCircle(CoordinateType::Map, x, y, r, Colors::Blue, false);
		break;
	case FollowSquad:
		Broodwar->drawCircle(CoordinateType::Map, x, y, r, Colors::Yellow, false);
		break;
	case FindTarget:
		Broodwar->drawCircle(CoordinateType::Map, x, y, r, Colors::Orange, false);
		break;
	case UserControl:
		Broodwar->drawCircle(CoordinateType::Map, x, y, r, Colors::Green, false);
		break;
	default:
		return;
	}
#endif
}

// Profile functions:
void
SmartAgent::userControl()
{
	if (!_unit->unit->isSelected())
		setProfile(Idle);
}

void
SmartAgent::flee()
{
	if (forceRatio() > _behavior["forceRatioToFlee"] && _hurtSpeed < 0.10 + _behavior["hurtSpeedToFlee"] * _lastHealth)
	{
		setProfile(FindTarget);
		findTarget();
		return;
	}
	_lastTarget = 0;
	_unit->unit->rightClick(_unit->lastPosition + (_threatVector * -100));
}

void
SmartAgent::findTarget()
{
	double bestScore = 0;
	UnitInfo* bestTarget = 0;

	UnitInfoSet targets(informationManager->closeEnemy(_unit->lastPosition, _behavior["targetRadius"]));
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

	if (bestScore > 0)
	{
		_target = bestTarget;
		DEBUG(" - Aquired target " << _target->id << ".");
		timeManager->now(_timeReevaluateTarget);
		_timeReevaluateTarget += timeManager->fromMs(_behavior["reevaluateTargetPeriod"]);
		setProfile(Fight);
		fight();
		return;
	}
	DEBUG(" - No target acquired.");
	_target = 0;
	_lastTarget = 0;
	if (forceRatio() <= _behavior["forceRatioToFlee"] || _hurtSpeed >= 0.10 + _behavior["hurtSpeedToFlee"] * _lastHealth)
	{
		setProfile(Flee);
		return;
	}
	if (_threatLevel <= _behavior["threatLevelToIdle"])
	{
		setProfile(Idle);
		return;
	}
}

double
SmartAgent::forceRatio()
{
	return (EPSILON + _forceLevel) / (EPSILON + _threatLevel);
}

void
SmartAgent::fight()
{
	if (forceRatio() <= _behavior["forceRatioToFlee"] || _hurtSpeed >= 0.10 + _behavior["hurtSpeedToFlee"] * _lastHealth)
	{
		targetManager->removeDamage(_lastTarget, _lastDamage);
		_lastDamage = 0;
		_lastTarget = 0;
		_target = 0;
		setProfile(Flee);
		return;
	}

	AttackInfo attack = informationManager->computeAttack(_unit, _target);

	if (!attack.possible || updateTime() > _timeReevaluateTarget)
	{
		targetManager->removeDamage(_lastTarget, _lastDamage);
		_lastDamage = 0;
		_target = 0;
		setProfile(FindTarget);
		findTarget();
		return;
	}

	// FIXME: Rewrite?
	if (_target != _lastTarget && (updateTime() > _timeCooldown || !_behavior["rangeOptimize"]))
	{
		DEBUG(" - Attacking target " << _target->id << ".");
		_unit->unit->rightClick(_target->unit);
		_timeCooldown = updateTime() + timeManager->fromMs(attack.cooldown);
		targetManager->addDamage(_target, attack.damage);
		_lastDamage = attack.damage;
		_lastTarget = _target;
		return;
	}
	else
	{
		if (_behavior["rangeOptimize"] && _timeCooldown - updateTime() >= timeManager->fromMs(_behavior["rangeOptimizeMinimumTime"]))
		{
			DEBUG(" - Moving to optimal range of target " << _target->id << ".");
			if (attack.currentRange > attack.optimalRange)
			{
				// Go towards the target
				_unit->unit->rightClick(_target->lastPosition);
				_lastTarget = 0;
			}
			else
			{
				// Go away from the target
				_unit->unit->rightClick(_unit->lastPosition - (_target->lastPosition - _unit->lastPosition));
				_lastTarget = 0;
			}
		}
		else
		{
			DEBUG(" - Continuing attack on " << _target->id << ".");
		}
	}
}

void
SmartAgent::idle()
{
	if (_squad)
	{
		setProfile(FollowSquad);
	}
	if (_threatLevel >= _behavior["threatLevelToFight"])
		setProfile(FindTarget);
}

void
SmartAgent::followSquad()
{
	if (!_squad)
	{
		setProfile(Idle);
		return;
	}
	if (_threatLevel >= _behavior["threatLevelToFight"])
		setProfile(FindTarget);

	switch (_squad->getOrder())
	{
	case UnitSquad::None:
		break;
	case UnitSquad::Expand:
		// FIXME: Do something.
		break;
	case UnitSquad::Tighten:
		_unit->unit->rightClick(_squad->getCenter());
		break;
	case UnitSquad::Move:
		{
			BWAPI::Position p = _squad->getTarget(this);
			if (_unit->lastPosition.getDistance(p) > _squad->getTargetRadius())
				_unit->unit->rightClick(p);
		}
		break;
	default:
		break;
	}
}

double
SmartAgent::computeTargetScore(UnitInfo* target)
{
	AttackInfo attack = informationManager->computeAttack(_unit, target);

	// NOTE: invisible units are not included.
	if (!attack.possible)
		return 0;

	double score = _behavior["baseScore"];
	VDEBUG("**** Target evaluation: " << target->id << ".");
	VDEBUG("Base score: " << score);

	// Range weighting.
	if (attack.inRange)
	{
		VDEBUG("In range: " << abs(attack.currentRange - attack.optimalRange) / (double) attack.maxRange * 2 * _behavior["weightOptimalRange"]);
		score += abs(attack.currentRange - attack.optimalRange) / (double) attack.maxRange * 2 * _behavior["weightOptimalRange"];
	}
	else
	{
		VDEBUG("Out of range: " << abs(attack.currentRange - attack.maxRange) / (double) attack.maxRange * _behavior["weightOutOfRange"]);
		// FIXME: What if too close?
		score += abs(attack.currentRange - attack.maxRange) / (double) attack.maxRange * _behavior["weightOutOfRange"];
	}

	// FIXME: add damage weighting.
	VDEBUG("Damage Factor: " << attack.damageFactor * _behavior["weightDamageFactor"]);
	score += attack.damageFactor * _behavior["weightDamageFactor"];

	// Unit hurt weighting.
	VDEBUG("Unit hurt: " << (target->unit->getHitPoints() + target->unit->getShields())
				    / (double) (target->lastType.maxHitPoints() + target->lastType.maxShields()) * _behavior["weightTargetHealth"]);
	score += (target->unit->getHitPoints() + target->unit->getShields())
				    / (double) (target->lastType.maxHitPoints() + target->lastType.maxShields()) * _behavior["weightTargetHealth"];

	// Threat level & targeting
	const TargetInfo& targetInfo = targetManager->targetInfo(target);
	double overkill = max(0, (targetInfo.damage + attack.damage) - targetInfo.health);
	if (overkill < 0)
		overkill *= -1;
	VDEBUG("Damage already on target: " << targetInfo.damage << "/" << targetInfo.health);
	VDEBUG("Threat: " << target->threatValue);
	VDEBUG("Overkill: " << overkill);
	VDEBUG("Weighted threat + overkill: " << (target->threatValue * _behavior["weightThreatLevel"])
	         + (overkill * _behavior["weightOverkill"]));
	score += (target->threatValue * _behavior["weightThreatLevel"])
	         + (overkill * _behavior["weightOverkill"]);
	VDEBUG("Final score:" << score);

	return score;
}
