#include "ProfileAgent.h"

using namespace BWAPI;

ProfileAgent::ProfileAgent(UnitInfo* unit, int customProfiles, TimeVal defaultUpdatePeriod, BehaviorManager::BehaviorType type)
: BehaviorAgent(unit, defaultUpdatePeriod, type),
	  _currentProfile(0)
{
	while (customProfiles > -1) // add +1 profile (default)
	{
		_profiles.push_back(defaultUpdatePeriod);
		--customProfiles;
	}
}

void
ProfileAgent::update()
{
	profileUpdate(_currentProfile);
}

void
ProfileAgent::profileUpdate(int)
{
}

void
ProfileAgent::setProfile(int profile)
{
	DEBUG(" - Switching to profile: " << profile);
	_currentProfile = profile;
	setUpdatePeriod(_profiles[_currentProfile]);
}
