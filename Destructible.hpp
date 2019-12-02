#ifndef DESCTRUCTIBLE_HPP
#define DESTRUCTIBLE_HPP


class Destructible : public Persistent
{

public:
	float m_max_hp;                                           // Maximum health
	float m_hp;                                               // Current health
	std::string m_corpse_name;								  // Name of the actor after its destroyed or killed
	int m_xp;                                                 // Xp given when killed (ar accumulated by player )
	ResistanceModel m_resistanceModel;


	Destructible(const float t_max_hp,
		const char* t_corpse_name,
		const int t_xp,
		const DmgArr& t_flat_resistances = make_DmgArr(0.0f),
		const DmgArr& t_mult_resistances = make_DmgArr(1.0f));

	Destructible& setFlatResistance(const DamageType t_dmg_type, const float t_flat);
	Destructible& setFlatResistance(const DmgArr& t_flat_list);
	Destructible& setMultResistance(const DamageType t_dmg_type, const float t_mult);
	Destructible& setMultResistance(const DmgArr& t_mult_list);
	float takeDamage(Actor* t_owner, const DamageType t_damage_type, float t_damage);
	inline bool isDead() { return m_hp <= 0; }
	virtual void die(Actor* t_owner);
	float heal(float t_amount);
	void save(TCODZip& t_zip);
	void load(TCODZip& t_zip);
	static std::unique_ptr<Destructible> create(TCODZip& t_zip);

protected:
	enum DestructibleType
	{
		MONSTER,
		PLAYER
	};

};

class MonsterDestructible : public Destructible
{
public:
	MonsterDestructible(const float t_max_hp, const char* t_corpse_name, const int t_xp, const DmgArr& t_flat_resistances = make_DmgArr(0.0f), const DmgArr& t_mult_resistances = make_DmgArr(1.0f));
	void die(Actor* t_owner);
	void save(TCODZip& t_zip);
};

class PlayerDestructible : public Destructible
{
public:
	PlayerDestructible(const float t_max_hp, const char* t_corpse_name, const int t_xp, const DmgArr& t_flat_resistances = make_DmgArr(0.0f), const DmgArr& t_mult_resistances = make_DmgArr(1.0f));
	void die(Actor* t_owner);
	void save(TCODZip& t_zip);
};

#endif