#pragma once

#include <BWAPI.h>

#include <set>

#include "Common.h"

#include "TimeLimiter.h"
#include "InformationManager.h"
#include "UnitInfo.h"
#include "UnitAgent.h"

#define DEFAULT_SQUAD_UPDATE_PERIOD 500

// Perform generic tactical tasks
//
// NOTE: automatically registers with InformationManager.
class UnitSquad
	: public TimeLimiter
{
public:
	enum Order
	{
		None = 0,
		Tighten = 1,
		Expand = 2,
		Move = 3
	};

	UnitSquad(Order order = None, BWAPI::Position target = BWAPI::Positions::None,
		        double targetRadius = 0, TimeVal updatePeriod = timeManager->fromMs(DEFAULT_SQUAD_UPDATE_PERIOD));
	virtual ~UnitSquad();

	virtual void displayInfo();

	virtual void frameUpdate();
	virtual void update();

	void computeStatistics();

	void addUnit(UnitAgent* unit);
	void removeUnit(UnitAgent* unit);
	inline const UnitAgentSet& getUnits() { return _units; }

	virtual BWAPI::Position getTarget(UnitAgent* who = 0);
	inline void setTarget(BWAPI::Position target) { _target = target; }
	inline double getTargetRadius() {return _targetRadius; }
	inline void setTargetRadius(double radius) { _targetRadius = radius; }

	inline double getSpread() { return _spread; }
	inline BWAPI::Position getCenter() { return _center; }
	inline Order getOrder() { return _order; }
	inline void setOrder(Order order) { _order = order; }

protected:
	UnitAgentSet _units;

	Order _order;
	BWAPI::Position _target;
	double _targetRadius;

	BWAPI::Position _center;
	double _spread; // Average distance to center.
};
