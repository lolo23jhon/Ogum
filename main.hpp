#ifndef MAIN_HPP
#define MAIN_HPP

#include "PreprocessorDirectives.hpp"
#include "libtcod.hpp"


namespace COL {

	constexpr const TCODColor& PLAYER_MSG_COLOR{ TCODColor::red };
	constexpr const TCODColor& NORMAL_MSG_COLOR{ TCODColor::lightGrey };

};

enum class ActorPreset { 
	PLAYER, 
	ORC, 
	TROLL 
};


enum class DamageType
{
	PHYSICAL,
	HEAT,
	COLD,
	ELECTRIC,
	MAX_DAMAGE_TYPES
};

#include <map>
#include <string>
#include <unordered_map>
#include <string.h>
#include <iterator>
#include <array>
#include <vector>
#include <algorithm>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#pragma comment(lib, "user32.lib")

#include "Utilities.hpp"
class Actor;
#include "Persistent.hpp"
#include "Destructible.hpp"
#include "Attacker.hpp"
#include "Ai.hpp"
#include "TargetSelector.hpp"
#include "Effect.hpp"
#include "Pickable.hpp"
#include "Container.hpp"
#include "Actor.hpp"
#include "Map.hpp"
#include "Gui.hpp"
#include "Engine.hpp"



#endif