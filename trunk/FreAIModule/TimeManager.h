#pragma once

#include <windows.h>

#include <BWAPI.h>

#include "Common.h"

// Frame period in ms:
// NOTE: The whole time will be available to the AI.
// If the game slows down because of something else,
// change the way time is allocated.
#define FRAME_PERIOD 40

// Handle the timing.
class TimeManager
{
public:
	TimeManager();

	void onStart();
	void onFrame();

	TimeVal now();
	void now(TimeVal& t);

	inline TimeVal fromMs(double ms) { return (TimeVal) (ms * _frequency / 1000); }
	inline TimeVal fromS(double s) { return (TimeVal) (s * _frequency); }
	inline double toMs(TimeVal t) { return ((double) t) * 1000 / _frequency; }
	inline double toS(TimeVal t) { return ((double) t) / _frequency; }

	inline TimeVal frameTime() { return _frameTime; }
	inline TimeVal frameLimit() { return _frameLimit; }
	inline TimeVal framePeriod() { return _framePeriod; }

private:
	TimeVal _frameTime;
	TimeVal _frameLimit;
	TimeVal _framePeriod;
	TimeVal _frequency;
};
