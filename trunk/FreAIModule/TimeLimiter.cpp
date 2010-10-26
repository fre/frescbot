#include "TimeLimiter.h"

TimeLimiter::TimeLimiter(TimeVal updatePeriod)
  : _timePeriod(updatePeriod)
{
	timeManager->now(_timeNext);
}

void
TimeLimiter::onFrame()
{
	frameUpdate();

	// Skip if too early to update.
	timeManager->now(_timeNow);
	if (_timeNow < _timeNext)
		return;

	update();

	// Schedule next update.
	// NOTE: schedule after processing, times might be more accurate.
	_timeNext = timeManager->now() + _timePeriod;
}

void
TimeLimiter::setUpdatePeriod(TimeVal updatePeriod)
{
	_timePeriod = updatePeriod;
}
