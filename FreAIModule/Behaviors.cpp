#include "Behaviors.h"

#include "Common.h"
#include "BehaviorManager.h"

namespace behaviors
{
	static
	std::map<const char*, double>*
	suicidalBehaviorMapInit()
	{
		std::map<const char*, double>* suicidalMap = new std::map<const char*, double>();
		(*suicidalMap)["defaultFightPeriod"] = 100;
		// Target choice score.
		(*suicidalMap)["weightOptimalRange"] = -1; // Penalize away from optimal range [0; 1].
		(*suicidalMap)["weightOutOfRange"] = -10; // Penalize out of range [0; inf * maxRange] = out of range %.
		(*suicidalMap)["weightDamageFactor"] = 50; // Prioritize target with the right armor type [0; 1].
		(*suicidalMap)["weightTargetHealth"] = -50; // Prioritize hurt targets [0; 1] = current health %.
		(*suicidalMap)["weightThreatLevel"] = 0.5; // Prioritize high threat targets [0; xxx] = a * price + b * damage.
		(*suicidalMap)["weightOverkill"] = -10; // Prioritize targets for precise kill [0; xxx] = abs(current health - (sum of attacks + ours)).
		// Behavior variables:
		(*suicidalMap)["reevaluateTargetPeriod"] = 200;
		(*suicidalMap)["rangeOptimize"] = 0;
		(*suicidalMap)["targetRadius"] = 1000;
		// Hurt speed computation & management:
		(*suicidalMap)["hurtSpeedToFlee"] = 1; // in % Max life/s, or >.
		return suicidalMap;
	}

	std::map<const char*, double>*
	suicidalBehaviorMap()
	{
		static std::map<const char*, double>* map = suicidalBehaviorMapInit();
		return map;
	}
}
