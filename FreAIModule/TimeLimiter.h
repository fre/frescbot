#pragma once

#include "TimeManager.h"

#define DEFAULT_UPDATE_PERIOD 500

// Base class for all classes with a set update period.
class TimeLimiter
{
public:
	TimeLimiter(TimeVal updatePeriod = timeManager->fromMs(DEFAULT_UPDATE_PERIOD));

	// Update agent if update period expired.
	virtual void frameUpdate() = 0; // Called every frame
	virtual void update() = 0; // Called every update.

	void onFrame(); // Call frameUpdate() and possible update().

	void setUpdatePeriod(TimeVal updatePeriod);
	inline TimeVal updatePeriod() { return _timePeriod; }
	inline TimeVal updateTime() { return _timeNow; }

private:
	// Updated automatically.
	TimeVal _timeNow;
	TimeVal _timePeriod;
	TimeVal _timeNext;
};
