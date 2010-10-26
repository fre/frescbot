#pragma once

// Helpful to avoid / 0 errors in non critical computations.
#define EPSILON 0.0001

// Display debug info on screen
#define DEBUG_DISPLAY 1
// #define DEBUG_LOG 1
// #define DEBUG_VERBOSE 1

// Log debug info
#ifdef DEBUG_LOG
# define DEBUG(Message) log << __FILE__ ":" << __LINE__ << ": " << Message << std::endl
# ifdef DEBUG_VERBOSE
#  define VDEBUG(Message) log << __FILE__ ":" << __LINE__ << ": " << Message << std::endl
# else
#  define VDEBUG(Message) void()	
# endif
#else
# define DEBUG(Message) void()
# define VDEBUG(Message) void()	
#endif

typedef __int64 TimeVal;

#ifdef DEBUG_LOG
#include <fstream>
extern std::ofstream log;
#endif

// Managers:
class TimeManager;
class InformationManager;
class ScoutManager;
class BehaviorManager;
class OverlayManager;
class TacticalManager;
class TargetManager;

// Initialized in AIModule::onStart();
extern TimeManager* timeManager;
extern InformationManager* informationManager;
extern ScoutManager* scoutManager;
extern BehaviorManager* behaviorManager;
extern OverlayManager* overlayManager;
extern TacticalManager* tacticalManager;
extern TargetManager* targetManager;