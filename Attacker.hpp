#ifndef ATTACKER_HPP
#define ATTACKER_HPP



class Attacker
{

protected:
	DamageType m_damage_type;
	TCOD_dice_t m_dice;
	std::string m_msg_verb; // "The skeleton punched, stabbed, cut... the player for x damage."


public:
	Attacker(const DamageType t_dmg_type, const char *t_dice, const char* t_verb= "attacked");
	void attack(Actor* t_owner, Actor* t_target);
	void save(TCODZip& t_zip);
	void load(TCODZip& t_zip);
};

class MeleeAttacker : public Attacker {
public:
	MeleeAttacker(const DamageType t_damage_type, const char * t_dice, const char* t_verb);

};

class RangedAttacker : public Attacker {
public:
	float m_range;

	RangedAttacker(const  DamageType t_damage_type, const char * t_dice, const char* t_verb,const float t_range);

};

#endif