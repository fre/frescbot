#pragma once

#include <vector>

#include <BWAPI.h>

#include "Common.h"

#include "UnitInfo.h"
#include "AttackInfo.h"
#include "BehaviorAgent.h"

class ProfileAgent : public BehaviorAgent
{
public:
	enum Profile
	{
		Default = 0
	};

	// customProfiles = additional profiles (max value of Profile enum).
	ProfileAgent(UnitInfo* unit, int customProfiles = 0, TimeVal defaultUpdatePeriod = timeManager->fromMs(DEFAULT_AGENT_UPDATE_PERIOD), BehaviorManager::BehaviorType type = BehaviorManager::DefaultBehavior);

	// Overloads from UnitAgent.
	void update(); // Do not overload in children.

	// Profile management.
	void setProfile(int profile);
	inline int getProfile() { return _currentProfile; }
	inline int getProfileCount() { return _profiles.size(); }
	inline void setProfileUpdatePeriod(int profile, TimeVal period) { _profiles[profile] = period; }

	virtual void profileUpdate(int profile); // Called each update cycle.

private:
	std::vector<TimeVal> _profiles;
	int _currentProfile;
};
