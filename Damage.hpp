#ifndef DAMAGE_HPP
#define DAMAGE_HPP



class Attack : Persistent {

	DamageType m_dmgType;
	std::string m_verb; // "The skeleton punched, stabbed, cut... the player for x damage."
	TCOD_dice_t m_dice;

public:
	Attack(const DamageType t_dmgType, const TCOD_dice_t t_dice, const char* t_verb = "attacked");
	const std::string& verb();
	int rollDmg();
	DamageType dmgType();
	void save(TCODZip& t_zip);
	void load(TCODZip& t_zip);

};

class ResistanceModel {

	struct ResistancePair {
		float m_flat;
		float m_mult;

		ResistancePair(float t_flat, float t_mult);
	};

	std::map<DamageType, ResistancePair> m_resistances;

public:
	ResistanceModel(const DmgArr& t_flat = make_DmgArr(0.0f), const DmgArr& t_mult = make_DmgArr(1.0f));
	float flatRes(const DamageType t_dmgType);
	float dmgMult(const DamageType t_dmgType);
	float reduceDamge(const DamageType t_dmgType, const float t_dmgMagnitude);
	void setFlatRes(const DmgArr& t_flat);
	void setFlatRes(const DamageType t_dmgType, const float t_value);
	void setMultRes(const DmgArr& t_mult);
	void setMultRes(const DamageType t_dmgType, const float t_value);

};


#endif