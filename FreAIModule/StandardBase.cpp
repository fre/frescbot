#include "StandardBase.h"

using namespace BWAPI;

#define BASE_WORKERS_PER_MINERAL 2
#define BASE_WORKERS_PER_GAS 3

StandardBase::StandardBase(TimeVal updatePeriod)
	: TimeLimiter(updatePeriod)
{
}

StandardBase::~StandardBase()
{
}

void
StandardBase::displayInfo()
{
}

void
StandardBase::frameUpdate()
{
}

void
StandardBase::update()
{
}

void
StandardBase::buildGas()
{
}

void
StandardBase::rebuild()
{
}

void
StandardBase::addWorker(UnitAgent* unit)
{
}
	
void
StandardBase::removeWorker(UnitAgent* unit)
{
}
