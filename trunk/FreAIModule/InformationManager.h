#pragma once

#include <windows.h>

#include <map>
#include <set>

#include <BWAPI.h>

#include "Common.h"

#include "TimeManager.h"
#include "TimeLimiter.h"

#include "UnitInfo.h"
#include "AttackInfo.h"
#include "UnitAgent.h"

// Time before assuming the unit is dead (ms).
#define UNIT_ASSUME_DEAD_PERIOD 5 * 60 * 1000
#define BUILDING_ASSUME_DEAD_PERIOD 20 * 60 * 1000

typedef std::set<UnitInfo*> UnitInfoSet;
typedef std::set<UnitAgent*> UnitAgentSet;
typedef std::map<BWAPI::Unit*, UnitInfo*> UnitInfoMap;
typedef std::map<BWAPI::Unit*, UnitAgent*> UnitAgentMap;
typedef std::set<UnitSquad*> UnitSquadSet;

// Manage unit information:
//  - Seen units.
//  - Potential targets and threats.
//  - Unit agents.
//
// Notes:
//  - Sets never contain units assumed dead.
//  - No UnitInfo* is deleted during the game.
//
//  - Consider renaming into UnitManager.
class InformationManager
	: public TimeLimiter
{
public:
	InformationManager();

	// Information update.
	void onStart();
  void onUnitCreate(BWAPI::Unit* unit);
  void onUnitDestroy(BWAPI::Unit* unit);
  void onUnitMorph(BWAPI::Unit* unit);
  void onUnitShow(BWAPI::Unit* unit);
  void onUnitHide(BWAPI::Unit* unit);
  void onUnitRenegade(BWAPI::Unit* unit);

	void frameUpdate();
	void update();

	// Set copy (with flags).
	// If slow, remove the extra duplication.
	UnitInfoSet all(int flags = UnitInfo::Visible);
	UnitInfoSet neutral(int flags = UnitInfo::Visible);
	UnitInfoSet self(int flags = UnitInfo::Visible);
	UnitInfoSet enemy(int flags = UnitInfo::Visible);

	// All unit agents
	inline const UnitAgentMap& agents() { return _agents; }

	// All unit squads
	inline const UnitSquadSet& squads() { return _squads; }
	inline void registerSquad(UnitSquad* squad) { _squads.insert(squad); }
	inline void unregisterSquad(UnitSquad* squad) { _squads.erase(squad); }

	// Reference to underlying sets.
	inline const UnitInfoSet& refAll() { return _all; }
	inline const UnitInfoSet& refNeutral() { return _neutral; }
	inline const UnitInfoSet& refSelf() { return _self; }
	inline const UnitInfoSet& refEnemy() { return _enemy; }

	// Closest neighbors.
	// FIXME: implement proper knn if bad results.
	UnitInfoSet closestAll(int n, BWAPI::Position position, int flags = UnitInfo::Visible);
	UnitInfoSet closestNeutral(int n, BWAPI::Position position, int flags = UnitInfo::Visible);
	UnitInfoSet closestSelf(int n, BWAPI::Position position, int flags = UnitInfo::Visible);
	UnitInfoSet closestEnemy(int n, BWAPI::Position position, int flags = UnitInfo::Visible);

	// Neighbors in radius
	UnitInfoSet closeAll(BWAPI::Position position, double radius, int flags = UnitInfo::Visible);
	UnitInfoSet closeNeutral(BWAPI::Position position, double radius, int flags = UnitInfo::Visible);
	UnitInfoSet closeSelf(BWAPI::Position position, double radius, int flags = UnitInfo::Visible);
	UnitInfoSet closeEnemy(BWAPI::Position position, double radius, int flags = UnitInfo::Visible);

	// Compute attack values.
	AttackInfo computeAttack(UnitInfo* unit, UnitInfo* target);

	// Display unit info.
	void displayUnitInfo(UnitInfo* unit);

	inline UnitInfo* unitInfo(BWAPI::Unit* unit) { return _map[unit]; }

private:
	UnitInfoSet _all;
	UnitInfoSet _self;
	UnitInfoSet _enemy;
	UnitInfoSet _neutral;

	UnitInfoMap _map;

	UnitAgentMap _agents;
	UnitSquadSet _squads;
};
