#pragma once

#include <BWAPI.h>

#include "Common.h"

// Store information on a unit.
struct UnitInfo
{
	enum Flags
	{
		None = 0,
		Visible = 1,
		Exists = 2,
		Enemy = 4,
		Self = 8,
		Neutral = 16,
		Building = 32,
		Unit = 64, // Not a building
		Air = 128,
		Ground = 256,
		Worker = 512,
		Mineral = 1024
	};

	int id;
	int flags;
	BWAPI::Unit* unit;
	double tacticalValue;
	double threatValue;

	// True if currently visible.
	inline bool visible()	{ return (flags & Visible) ? true : false; }
	// True until assumed dead.
	inline bool exists()	{ return (flags & Exists) ? true : false; }

	TimeVal lastTime;
	BWAPI::UnitType lastType;
	BWAPI::Position lastPosition;
};
