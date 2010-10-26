#pragma once

#include <BWAPI.h>
#include <BWTA.h>

#include <set>

#include "Common.h"

#include "TimeLimiter.h"
#include "InformationManager.h"
#include "UnitInfo.h"

#define DEFAULT_BASE_UPDATE_PERIOD 500

// Handle resource balancing / worker management in a base/expansion
class StandardBase
	: public TimeLimiter
{
public:
	StandardBase(TimeVal updatePeriod = timeManager->fromMs(DEFAULT_BASE_UPDATE_PERIOD));
	virtual ~StandardBase();

	virtual void displayInfo();

	virtual void frameUpdate();
	virtual void update();

	void buildGas();
	void rebuild();

	void addWorker(UnitAgent* unit);
	void removeWorker(UnitAgent* unit);
	inline const UnitAgentSet& getWorkers() { return _workers; }

protected:
	UnitAgentSet _workers;
	UnitAgentSet _mineralWorkers;
	UnitAgentSet _gasWorkers;
	UnitInfoSet _resourceTargets;
	UnitInfo* _building;

	// Estimated
	double _mineralIncome;
	double _gasIncome;

	int _mineralsLeft;
	int _gasLeft;

	BWTA::BaseLocation* _baseLocation;
};
