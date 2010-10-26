#pragma once

#include <BWAPI.h>

#include "Common.h"

#include "Vector2D.h"
#include "ProfileAgent.h"
#include "InformationManager.h"

// Smart ranged agent, can retreat.
//
// Implementation: simple state machine.
// Be careful not to introduce infinite loops in the same frame.
//
class SmartAgent
	: public ProfileAgent
{
public:
	enum Profile
	{
		Default = 0,
		Idle = 1,
		FindTarget = 2,
		Fight = 3,
		Flee = 4,
		FollowSquad = 5,
		UserControl = 6
	};

	SmartAgent(UnitInfo* unit, int customProfiles = 6, BehaviorManager::BehaviorType type = BehaviorManager::SmartBehavior);

	virtual void profileUpdate(int profile);

	// Display debug info, e.g. attack lines.
	virtual void displayDebugInfo();

	// Profile functions:
	virtual void flee();
	virtual void findTarget();
	virtual void fight();
	virtual void idle();
	virtual void followSquad();
	virtual void userControl();

	// Helper functions:
	virtual void assessThreatLevel();
	virtual void assessForceLevel();
	virtual void assessHurtSpeed();
	virtual double computeTargetScore(UnitInfo* target);

	virtual double forceRatio();

protected:
	TimeVal _timeCooldown;
	TimeVal _timeReevaluateTarget;

	UnitInfo* _target;
	UnitInfo* _lastTarget;
	double _lastDamage;

	UnitInfoSet _threats;
	double _threatLevel;
	Vector2D _threatVector;
	UnitInfoSet _forces;
	double _forceLevel;
	Vector2D _forceVector;

	double _lastHealth; // within [0; 1].
	TimeVal _timeHealth;
	double _hurtSpeed;
	std::vector<double> _hurtSpeedSamples;
	int _hurtSpeedSample;
	TimeVal _hurtSpeedPeriod;
};