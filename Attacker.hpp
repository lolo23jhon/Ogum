#ifndef ATTACKER_HPP
#define ATTACKER_HPP



class Attacker
{
protected:
	Attack m_attack;


public:

	Attacker(const DamageType t_dmgType, const TCOD_dice_t t_dice, const char* t_verb);
	void attack(Actor* t_owner, Actor* t_target);
	void save(TCODZip& t_zip);
	void load(TCODZip& t_zip);

	static std::unique_ptr<Attacker> create(TCODZip& t_zip);

	friend class Actor;
};



#endif