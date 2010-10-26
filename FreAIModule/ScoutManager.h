#pragma once

#include <windows.h>

#include <BWAPI.h>
#include <BWTA.h>

#include "Common.h"

#include "TimeManager.h"
#include "TimeLimiter.h"

#include "ScoutInfo.h"
#include "RefMap.h"

typedef RefMap<double> HeatMap;
typedef RefMap<ScoutInfo> ScoutInfoMap;

// Gather map information & player-related statistics & infos.
// Maintain one scout target.
class ScoutManager
	: public TimeLimiter
{
public:
	// Information update.
	void onStart();

	void update();
	void frameUpdate();

	void updateScoutMap();
	void updateForceMap();
	void updateThreatMap();
	void updateTacticalMap();

	void displayInfo();

	std::set<std::pair<double, ScoutInfo*> > getScoutTargets(unsigned targetCount = 1, bool reachable = true);

private:
	// Map dimensions.
	int _mapWidth;
	int _mapHeight;
	int _mapTileWidth;
	int _mapTileHeight;
	int _mapBinWidth;
	int _mapBinHeight;
	int _tacticalMapBinWidth;
	int _tacticalMapBinHeight;

	// Information maps.
	HeatMap _threatMap;
	HeatMap _forceMap;
	HeatMap _tacticalMap;
	HeatMap _terrainMap;
	ScoutInfoMap _scoutMap;

	// Important positions
	BWAPI::Position _startLocation;

	// Set after map is analyzed.
	bool _analyzed;
};
