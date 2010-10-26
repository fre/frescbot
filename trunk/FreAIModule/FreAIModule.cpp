#include "FreAIModule.h"

#include "TimeManager.h"
#include "InformationManager.h"
#include "ScoutManager.h"
#include "OverlayManager.h"
#include "BehaviorManager.h"
#include "TacticalManager.h"
#include "TargetManager.h"

#include "UnitInfo.h"
#include "UnitAgent.h"
#include "UnitSquad.h"

using namespace BWAPI;

FreAIModule::FreAIModule()
{
}

FreAIModule::~FreAIModule()
{
}

void FreAIModule::onStart()
{
	if (Broodwar->isReplay())
	{
	  Broodwar->enableFlag(Flag::UserInput);
		return;
	}
#ifdef DEBUG_LOG
	// Open log file.
	log.open("FreAIModule.log");

	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		DEBUG(MODULE_NAME << " version " << MODULE_VERSION);
		DEBUG(time.wYear << "/" << time.wMonth << "/" << time.wDay << " " << time.wHour << ":" << time.wMinute << ":" << time.wSecond);
		DEBUG("--");
	}
#endif

  Broodwar->sendText("This is " MODULE_NAME " version " MODULE_VERSION ".");
  Broodwar->sendText("gl hf");

  // Enable some cheat flags
  // Broodwar->enableFlag(Flag::UserInput);
  // Uncomment to enable complete map information
  Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Create managers.
	timeManager = new TimeManager();
	overlayManager = new OverlayManager();
	behaviorManager = new BehaviorManager();
	informationManager = new InformationManager();
	scoutManager = new ScoutManager();
	tacticalManager = new TacticalManager();
	targetManager = new TargetManager();

	// Start managers.
	timeManager->onStart();
	behaviorManager->onStart();
	informationManager->onStart();
	scoutManager->onStart();
	tacticalManager->onStart();
	targetManager->onStart();
}

void FreAIModule::onEnd(bool isWinner)
{
	if (Broodwar->isReplay())
		return;
	if (isWinner)
	{
		//log win to file
	}
#ifdef DEBUG_LOG
	log.close();
#endif
}

void FreAIModule::onFrame()
{
	if (Broodwar->isReplay())
		return;
	// Using BWAPI in threads crashes the game.
	// Let's keep everything in onFrame() and only run long computations in threads.

	timeManager->onFrame();

	DEBUG("Frame " << Broodwar->getFrameCount() << ":");

	// Do mandatory operations.
	DEBUG("-- Information update --");
	informationManager->onFrame();
	DEBUG("-- Scout update --");
	scoutManager->onFrame();
	DEBUG("-- Tactical update --");
	tacticalManager->onFrame();
	DEBUG("-- Target update --");
	targetManager->onFrame();

	// Update squads.
	DEBUG("-- Squad update --");
	const UnitSquadSet& squads(informationManager->squads());
	for (UnitSquadSet::const_iterator it = squads.begin();
			 it != squads.end(); ++it)
	{
		UnitSquad* squad = *it;
		squad->onFrame();
	}

	// Update agents.
	DEBUG("-- Agent update --");
	const UnitAgentMap& agents(informationManager->agents());
	for (UnitAgentMap::const_iterator it = agents.begin();
			 it != agents.end(); ++it)
	{
		UnitAgent* agent = it->second;
		agent->onFrame();
	}

	// Put in last so that controls are on top of the rest.
	DEBUG("-- Overlay update --");
	overlayManager->onFrame();

	{
		// Check remaining time.
		// FIXME: If time remaining is negative, adjust update frequencies.
		TimeVal left = timeManager->frameLimit() - timeManager->now() ;
		DEBUG("Frame time remaining: " << timeManager->toMs(left) << "ms.");
#ifdef DEBUG_DISPLAY
		// Show performance on screen.
		Broodwar->drawCircle(CoordinateType::Screen, 6, 6, 4, Colors::Black, true);
		Broodwar->drawBoxScreen(8, 5, 110, 7, Colors::Black, true);
		Color c = Color(max(0, 255 - (int) (255 * left / (double) timeManager->framePeriod())),
			              min(255, (int) (255 * left / (double) timeManager->framePeriod())), 0);
		Broodwar->drawCircle(CoordinateType::Screen, 6, 6, 3, c, true);
		Broodwar->drawLine(CoordinateType::Screen, 9, 6, max(9, (int) (100 * left / (double) timeManager->framePeriod()) + 9), 6, c);
#endif
	}
}

void FreAIModule::onUnitCreate(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay())
		return;
	informationManager->onUnitCreate(unit);
}

void FreAIModule::onUnitDestroy(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay())
		return;
	informationManager->onUnitDestroy(unit);
}

void FreAIModule::onUnitMorph(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay())
		return;
	informationManager->onUnitMorph(unit);
}

void FreAIModule::onUnitShow(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay())
		return;
	informationManager->onUnitShow(unit);
}

void FreAIModule::onUnitHide(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay())
		return;
	informationManager->onUnitHide(unit);
}

void FreAIModule::onUnitRenegade(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay())
		return;
	informationManager->onUnitRenegade(unit);
}

void FreAIModule::onPlayerLeft(BWAPI::Player* player)
{
	if (Broodwar->isReplay())
		return;
  Broodwar->sendText("%s left the game.",player->getName().c_str());
}

void FreAIModule::onNukeDetect(BWAPI::Position target)
{
	if (Broodwar->isReplay())
		return;
  if (target != Positions::Unknown)
    Broodwar->printf("Nuclear Launch Detected at (%d,%d)",target.x(),target.y());
  else
    Broodwar->printf("Nuclear Launch Detected");
}

bool FreAIModule::onSendText(std::string text)
{
	if (Broodwar->isReplay())
		return true;
	return true;
}
