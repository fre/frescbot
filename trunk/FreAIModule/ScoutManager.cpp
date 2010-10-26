#include "ScoutManager.h"

#include "InformationManager.h"
#include "OverlayManager.h"

using namespace BWAPI;

#define BINS_PER_TILE 1
#define TACTICAL_BINS_PER_TILE 0.125

#define SCOUT_COOL_SPEED 0.005 // in [0; 1].
#define SCOUT_DEFAULT_VALUE 1
#define SCOUT_CENTER_DISTANCE_VALUE -0.1
#define SCOUT_EXPAND_LOCATION_VALUE 3
#define SCOUT_START_LOCATION_VALUE 2 // Adds up

// Terrain weights
#define TERRAIN_CHOKE_POINT_VALUE 0.0001
#define TERRAIN_WALL_VALUE 0.01

// Display info
#define TACTICAL_BOX_WIDTH 3
#define TACTICAL_BOX_HEIGHT 10
#define THREAT_BOX_WIDTH 10
#define THREAT_BOX_HEIGHT 3

// Unit maps heating/cooling values (/s).
#define FORCE_COOL_SPEED 1
#define FORCE_HEAT_FACTOR 0.01
#define THREAT_COOL_SPEED 0.2
#define THREAT_HEAT_FACTOR 0.01
#define TACTICAL_COOL_SPEED 0.01
#define TACTICAL_HEAT_FACTOR 0.01

static DWORD WINAPI
analyzeMap(void* p)
{
	void** parameters = reinterpret_cast<void**>(p);
	bool* analyzed = reinterpret_cast<bool*>(parameters[0]);
	Position* startLocation = reinterpret_cast<Position*>(parameters[1]);
	ScoutInfoMap* scoutMap = reinterpret_cast<ScoutInfoMap*>(parameters[2]);
	HeatMap* terrainMap = reinterpret_cast<HeatMap*>(parameters[3]);
	delete[] parameters;

  BWTA::analyze();

	// Terrain map update
	for (int y = 0; y < terrainMap->yBins(); ++y)
		for (int x = 0; x < terrainMap->xBins(); ++x)
		{
			double& v(terrainMap->bin(x, y));
			Position pos(terrainMap->binCenter(x, y));
			BWTA::Chokepoint* choke = BWTA::getNearestChokepoint(pos);
			if (choke)
			{
				Position chokeCenter = choke->getCenter();
				// FIXME: double check it's ok to run from a thread.
				// NOTE: using BWTA::getGroundDistance() sometimes crashes.
				double distance = pos.getDistance(chokeCenter);
				v *= min(1, TERRAIN_CHOKE_POINT_VALUE * distance * choke->getWidth());
			}
			Position wallPos = BWTA::getNearestUnwalkablePosition(pos);
			// FIXME: double check it's ok to run from a thread.
			// NOTE: using BWTA::getGroundDistance() sometimes crashes.
			double wallDistance = pos.getDistance(wallPos);
			v *= min(1, TERRAIN_WALL_VALUE * wallDistance);
		}

	double diagLength = Position(scoutMap->width(), scoutMap->height()).getLength();
	// Scout map update
	for (int y = 0; y < scoutMap->yBins(); ++y)
	{
		for (int x = 0; x < scoutMap->xBins(); ++x)
		{
			ScoutInfo& val(scoutMap->bin(x, y));
			Position pos(scoutMap->binCenter(x, y));
			val.position = pos;
			val.value = SCOUT_DEFAULT_VALUE * terrainMap->bin(x, y); // More important to scout better tactical terrain
			val.value += Position((pos.x() - scoutMap->width() / 2),
														(pos.y() - scoutMap->height() / 2)).getLength()
									/ diagLength * SCOUT_CENTER_DISTANCE_VALUE; // More important to scout central terrain.
			if (BWTA::isConnected(*startLocation, pos))
				val.reachable = true;
		}
	}

  // Value bases & expansions
	const std::set<BWTA::BaseLocation*>& bases = BWTA::getStartLocations();
	for (std::set<BWTA::BaseLocation*>::const_iterator it = bases.begin(); it != bases.end(); ++it)
		(*scoutMap)[(*it)->getPosition()].value += SCOUT_START_LOCATION_VALUE;
	const std::set<BWTA::BaseLocation*>& expands = BWTA::getBaseLocations();
	for (std::set<BWTA::BaseLocation*>::const_iterator it = expands.begin(); it != expands.end(); ++it)
		(*scoutMap)[(*it)->getPosition()].value += SCOUT_EXPAND_LOCATION_VALUE;

	*analyzed = true;
	return 0;
}

static void
readHeightMap(HeatMap& map)
{
	for (int y = 0; y < map.yBins(); ++y)
		for (int x = 0; x < map.xBins(); ++x)
		{
			double& v(map.bin(x, y));
			Position p(map.binCenter(x, y));
			v = (1 + Broodwar->getGroundHeight(p.x() / (TILE_SIZE / 4), p.y() / (TILE_SIZE / 4))) / 3.; // in walk tiles
		}
}

void
ScoutManager::onStart()
{
	// Map dimensions
	_mapTileWidth = Broodwar->mapWidth();
	_mapTileHeight = Broodwar->mapHeight();
	_mapWidth = _mapTileWidth * TILE_SIZE;
	_mapHeight = _mapTileHeight * TILE_SIZE;
	_mapBinWidth = (int) (_mapTileWidth * BINS_PER_TILE);
	_mapBinHeight = (int) (_mapTileHeight * BINS_PER_TILE);
	_tacticalMapBinWidth = (int) (_mapTileWidth * TACTICAL_BINS_PER_TILE);
	_tacticalMapBinHeight = (int) (_mapTileWidth * TACTICAL_BINS_PER_TILE);

	// Initialize maps.
	_threatMap = HeatMap(_mapWidth, _mapHeight, _mapBinWidth, _mapBinHeight, 0);
	_forceMap = HeatMap(_mapWidth, _mapHeight, _mapBinWidth, _mapBinHeight, 0);
	_tacticalMap = HeatMap(_mapWidth, _mapHeight, _tacticalMapBinWidth, _tacticalMapBinHeight, 0);
	_terrainMap = HeatMap(_mapWidth, _mapHeight, _mapBinWidth, _mapBinHeight, 0);
	_scoutMap = ScoutInfoMap(_mapWidth, _mapHeight, _mapBinWidth, _mapBinHeight);

	_startLocation = Broodwar->self()->getStartLocation();

	DEBUG("** Beginning map analysis **");
	_analyzed = 0;
	readHeightMap(_terrainMap);
	BWTA::readMap();
	void** parameters = new void*[4];
	parameters[0] = &_analyzed;
	parameters[1] = &_startLocation;
	parameters[2] = &_scoutMap;
	parameters[3] = &_terrainMap;
  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)analyzeMap, parameters, 0, NULL);
}

void
ScoutManager::frameUpdate()
{
#ifdef DEBUG_DISPLAY
	displayInfo();
#endif
}

void
ScoutManager::update()
{
	static bool analyzeJustFinished = true;
	// Values are being written by the analysis.
	if (!_analyzed)
		return;
	if (analyzeJustFinished)
	{
		analyzeJustFinished = false;
		DEBUG("** Map analysis completed. **");
	}

	updateScoutMap();
	updateTacticalMap();
	updateThreatMap();
	updateForceMap();
}

void
ScoutManager::updateScoutMap()
{
	double heatLoss = (SCOUT_COOL_SPEED * DEFAULT_UPDATE_PERIOD / 1000);
	for (int y = 0; y < _scoutMap.yBins(); ++y)
		for (int x = 0; x < _scoutMap.xBins(); ++x)
		{
			ScoutInfo& p(_scoutMap.bin(x, y));
			Position pos = _scoutMap.binCenter(x, y);
			if (Broodwar->isVisible(pos))
				p.heat = 1;
			else
				p.heat = max(0, p.heat - heatLoss);
			// FIXME: update values.
		}
}


void
ScoutManager::updateForceMap()
{
	double heatLoss = (FORCE_COOL_SPEED * DEFAULT_UPDATE_PERIOD / 1000);
	for (int y = 0; y < _forceMap.yBins(); ++y)
		for (int x = 0; x < _forceMap.xBins(); ++x)
		{
			double& v(_forceMap.bin(x, y));
			Position pos = _forceMap.binCenter(x, y);
			if (Broodwar->isVisible(pos))
				v = 0;
			else
				v = max(0, min(1, v - heatLoss));
		}

	UnitInfoSet units = informationManager->self();
	for (UnitInfoSet::iterator it = units.begin(); it != units.end(); ++it)
	{
		UnitInfo* info = *it;
		if (info->visible())
			_forceMap[info->lastPosition] += FORCE_HEAT_FACTOR * info->threatValue;
	}
}

void
ScoutManager::updateThreatMap()
{
	double heatLoss = (THREAT_COOL_SPEED * DEFAULT_UPDATE_PERIOD / 1000);
	for (int y = 0; y < _threatMap.yBins(); ++y)
		for (int x = 0; x < _threatMap.xBins(); ++x)
		{
			double& v(_threatMap.bin(x, y));
			Position pos = _threatMap.binCenter(x, y);
			if (Broodwar->isVisible(pos))
				v = 0;
			else
				v = max(0, min(1, v - heatLoss));
		}

	UnitInfoSet units = informationManager->enemy();
	for (UnitInfoSet::iterator it = units.begin(); it != units.end(); ++it)
	{
		UnitInfo* info = *it;
		if (info->visible())
			_threatMap[info->lastPosition] += FORCE_HEAT_FACTOR * info->threatValue;
	}
}

void
ScoutManager::updateTacticalMap()
{
	double heatLoss = (TACTICAL_COOL_SPEED * DEFAULT_UPDATE_PERIOD / 1000);
	for (int y = 0; y < _tacticalMap.yBins(); ++y)
		for (int x = 0; x < _tacticalMap.xBins(); ++x)
		{
			double& v(_tacticalMap.bin(x, y));
			Position pos = _tacticalMap.binCenter(x, y);
			if (Broodwar->isVisible(pos))
				v = 0;
			else
				v = max(0, min(1, v - heatLoss));
		}

	UnitInfoSet units = informationManager->enemy();
	for (UnitInfoSet::iterator it = units.begin(); it != units.end(); ++it)
	{
		UnitInfo* info = *it;
		if (info->visible())
			_tacticalMap[info->lastPosition] += FORCE_HEAT_FACTOR * info->threatValue;
	}
}

std::set<std::pair<double, ScoutInfo*> >
ScoutManager::getScoutTargets(unsigned targetCount, bool reachable)
{
	std::set<std::pair<double, ScoutInfo*> > result;
	double minValue = 0;

	for (int y = 0; y < _scoutMap.yBins(); ++y)
		for (int x = 0; x < _scoutMap.xBins(); ++x)
		{
			ScoutInfo& p(_scoutMap.bin(x, y));
			if (reachable && !p.reachable)
				continue;
			double v = p.value * (1 - p.heat);
			if (v > minValue || result.size() < targetCount)
			{
				result.insert(std::make_pair(v, &p));
				if (result.size() > targetCount)
					result.erase(result.begin());
				minValue = result.begin()->first;
			}
		}

	return result;
}

void
ScoutManager::displayInfo()
{
  if (!_analyzed)
		return;

	// Display scout map
	if (overlayManager->overlay(OverlayManager::ScoutOverlay))
	{
		for (int y = 0; y < _scoutMap.yBins(); ++y)
			for (int x = 0; x < _scoutMap.xBins(); ++x)
			{
				ScoutInfo& p(_scoutMap.bin(x, y));
				Position pos = _scoutMap.binCenter(x, y);
				if (p.reachable)
				{
					int v = min(255, max(0, (int) (p.heat * 170)));
					Color c(v, v, v);
					Broodwar->drawCircle(CoordinateType::Map, pos.x(), pos.y(), 3, c, true);
				}
				else
					Broodwar->drawCircle(CoordinateType::Map, pos.x(), pos.y(), 3, Colors::Grey, true);					
			}
	}

	// Display terrain map
	if (overlayManager->overlay(OverlayManager::TerrainOverlay))
	{
		for (int y = 0; y < _terrainMap.yBins(); ++y)
			for (int x = 0; x < _terrainMap.xBins(); ++x)
			{
				double& p(_terrainMap.bin(x, y));
				Position pos = _terrainMap.binCenter(x, y);
				int v = (int) (p * 255);
				Color c(min(255, max(0, 255 - v)), min(255, max(0, v)), 0);
				Broodwar->drawCircle(CoordinateType::Map, pos.x(), pos.y(), 2, c, true);
			}
	}

	// Display threat and forces maps
	if (overlayManager->overlay(OverlayManager::ThreatOverlay))
	{
		for (int y = 0; y < _threatMap.yBins(); ++y)
			for (int x = 0; x < _threatMap.xBins(); ++x)
			{
				double v(_threatMap.bin(x, y));
				Position pos = _threatMap.binCenter(x, y);
				int boxX = pos.x() - (int) (v * THREAT_BOX_WIDTH);
				int boxY = pos.y() - THREAT_BOX_HEIGHT / 2;
				int boxEndX = pos.x();
				int boxEndY = boxY + THREAT_BOX_HEIGHT;
				Broodwar->drawBox(CoordinateType::Map, boxX, boxY, boxEndX, boxEndY, Colors::Red, true);
			}
		for (int y = 0; y < _forceMap.yBins(); ++y)
			for (int x = 0; x < _forceMap.xBins(); ++x)
			{
				double v(_forceMap.bin(x, y));
				Position pos = _forceMap.binCenter(x, y);
				int boxX = pos.x();
				int boxY = pos.y() - THREAT_BOX_HEIGHT / 2;
				int boxEndX = pos.x() + (int) (v * THREAT_BOX_WIDTH);
				int boxEndY = boxY + THREAT_BOX_HEIGHT;
				Broodwar->drawBox(CoordinateType::Map, boxX, boxY, boxEndX, boxEndY, Colors::Blue, true);
			}
	}

	// Display tactical maps
	if (overlayManager->overlay(OverlayManager::TacticalOverlay))
	{
		for (int y = 0; y < _tacticalMap.yBins(); ++y)
			for (int x = 0; x < _tacticalMap.xBins(); ++x)
			{
				double v(_tacticalMap.bin(x, y));
				Position pos = _tacticalMap.binCenter(x, y);
				int boxX = pos.x() - TACTICAL_BOX_WIDTH / 2;
				int boxY = pos.y() - (int) (v * TACTICAL_BOX_HEIGHT);
				int boxEndX = boxX + TACTICAL_BOX_WIDTH;
				int boxEndY = pos.y();
				Broodwar->drawBox(CoordinateType::Map, boxX, boxY, boxEndX, boxEndY, Colors::Green, true);
			}
	}

	// Display BWTA information (code from the BWTA example)
	// Iterate through all the base locations and draw their outlines.
	if (overlayManager->overlay(OverlayManager::BWTAOverlay))
	{
	  for(std::set<BWTA::BaseLocation*>::const_iterator i = BWTA::getBaseLocations().begin();
				i != BWTA::getBaseLocations().end(); ++i)
		{
	    TilePosition p = (*i)->getTilePosition();
			Position c = (*i)->getPosition();

			// Draw outline of center location
			Broodwar->drawBox(CoordinateType::Map, p.x() * 32, p.y() * 32, p.x() * 32 + 4 * 32, p.y() * 32 + 3 * 32, Colors::Blue, false);

			// Draw a circle at each mineral patch
			for(std::set<BWAPI::Unit*>::const_iterator j = (*i)->getStaticMinerals().begin(); j != (*i)->getStaticMinerals().end(); ++j)
			{
	      Position q = (*j)->getInitialPosition();
				Broodwar->drawCircle(CoordinateType::Map, q.x(), q.y(), 30, Colors::Cyan, false);
			}

			// Draw the outlines of vespene geysers
			for(std::set<BWAPI::Unit*>::const_iterator j = (*i)->getGeysers().begin(); j != (*i)->getGeysers().end(); ++j)
			{
	      TilePosition q = (*j)->getInitialTilePosition();
				Broodwar->drawBox(CoordinateType::Map, q.x() * 32, q.y() * 32, q.x() * 32 + 4 * 32, q.y() * 32 + 2 * 32, Colors::Orange, false);
			}

			// If this is an island expansion, draw a yellow circle around the base location
			if ((*i)->isIsland())
			{
	      Broodwar->drawCircle(CoordinateType::Map, c.x(), c.y(), 80, Colors::Yellow, false);
			}
		}
    
	  // Iterate through all the regions and draw their polygon outline in green.
	  for(std::set<BWTA::Region*>::const_iterator r = BWTA::getRegions().begin(); r != BWTA::getRegions().end(); ++r)
	  {
			BWTA::Polygon p = (*r)->getPolygon();
			for(int j = 0; j < (int)p.size(); ++j)
			{
	      Position point1 = p[j];
				Position point2 = p[(j + 1) % p.size()];
				Broodwar->drawLine(CoordinateType::Map, point1.x(), point1.y(), point2.x(), point2.y(), Colors::Green);
			}
		}

	  // Visualize the chokepoints with red lines
	  for(std::set<BWTA::Region*>::const_iterator r = BWTA::getRegions().begin(); r != BWTA::getRegions().end(); ++r)
	  {
			for(std::set<BWTA::Chokepoint*>::const_iterator c = (*r)->getChokepoints().begin(); c != (*r)->getChokepoints().end(); ++c)
			{
	      Position point1 = (*c)->getSides().first;
				Position point2 = (*c)->getSides().second;
				Broodwar->drawLine(CoordinateType::Map, point1.x(), point1.y(), point2.x(), point2.y(), Colors::Red);
			}
		}
	}
}
