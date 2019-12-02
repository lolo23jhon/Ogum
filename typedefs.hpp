#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP


namespace COL {

	static const TCODColor& PLAYER_MSG_COLOR{ TCODColor::red };
	static const TCODColor& NORMAL_MSG_COLOR{ TCODColor::lightGrey };

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

using DmgArr = std::array<float, (unsigned int)DamageType::MAX_DAMAGE_TYPES>;


static DmgArr make_DmgArr(float t_fill) {
	DmgArr arr;
	for (int i{ 0 }; i < (int)DamageType::MAX_DAMAGE_TYPES; i++) {
		arr[i] = t_fill;
	}
	return std::move(arr);
}



#endif
