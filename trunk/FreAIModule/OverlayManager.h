#pragma once

#include <windows.h>

#include <BWAPI.h>

#include <vector>

#include "Common.h"

#include "TimeLimiter.h"

class OverlayManager
	: public TimeLimiter
{
public:
	enum Overlay
	{
		ScoutOverlay = 0,
		TerrainOverlay,
		BWTAOverlay,
		ThreatOverlay,
		TacticalOverlay,
		UnitInfoOverlay,
		AgentOverlay,
		SquadOverlay,
		OverlayCount
	};

	OverlayManager();

	void frameUpdate();
	void update();

	inline bool overlay(Overlay o) { return _overlays[o]; }
private:
	std::vector<bool> _overlays;
	std::vector<BWAPI::Color> _overlayColors;
	bool _clicked;
};
