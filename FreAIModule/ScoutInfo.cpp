#include "ScoutInfo.h"

ScoutInfo::ScoutInfo(double h, double v, bool r, BWAPI::Position p)
  : heat(h),
	  value(v),
		reachable(r),
		position(p)
{
}
