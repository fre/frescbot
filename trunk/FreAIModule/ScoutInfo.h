#pragma once

#include <BWAPI.h>

struct ScoutInfo
{
	ScoutInfo(double heat = 0, double value = 0, bool reachable = false, BWAPI::Position position = BWAPI::Positions::None);

	double heat;
	double value;
	bool reachable;
	BWAPI::Position position;
};
