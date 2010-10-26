#pragma once

#include <windows.h>
#include <process.h>

#include <vector>
#include <fstream>
#include <iostream>

#include <BWAPI.h>

#include "Common.h"

#define MODULE_NAME "FreScBot"
#define MODULE_VERSION "1.0"

class FreAIModule : public BWAPI::AIModule
{
public:
	FreAIModule();
	~FreAIModule();

  virtual void onStart();
  virtual void onEnd(bool isWinner);
  virtual void onFrame();
  virtual bool onSendText(std::string text);
  virtual void onPlayerLeft(BWAPI::Player* player);
  virtual void onNukeDetect(BWAPI::Position target);
  virtual void onUnitCreate(BWAPI::Unit* unit);
  virtual void onUnitDestroy(BWAPI::Unit* unit);
  virtual void onUnitMorph(BWAPI::Unit* unit);
  virtual void onUnitShow(BWAPI::Unit* unit);
  virtual void onUnitHide(BWAPI::Unit* unit);
  virtual void onUnitRenegade(BWAPI::Unit* unit);
};
