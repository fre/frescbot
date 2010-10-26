#include "TimeManager.h"

#define GET_TIME(TimeValP) QueryPerformanceCounter((LARGE_INTEGER*) TimeValP)

TimeManager::TimeManager()
{
	if (!QueryPerformanceFrequency((LARGE_INTEGER*) &_frequency))
	{
		DEBUG("** WARNING: No high performance counter available **");
	}
}

void
TimeManager::onStart()
{
	GET_TIME(&_frameLimit);
	_framePeriod = fromMs(FRAME_PERIOD);
}

void
TimeManager::onFrame()
{
	GET_TIME(&_frameTime);
	// Allocate a full period for the frame.
	_frameLimit = _frameTime + _framePeriod;
}


void
TimeManager::now(TimeVal& t)
{
	QueryPerformanceCounter((LARGE_INTEGER*) &t);
}

TimeVal
TimeManager::now()
{
	TimeVal t;
	QueryPerformanceCounter((LARGE_INTEGER*) &t);
	return t;
}
