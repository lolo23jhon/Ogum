#include "main.hpp"


Destructible::Destructible(
	const float t_max_hp,
	const char* t_corpse_name,
	const int t_xp,
	const DmgArr& t_flatRes,
	const DmgArr& t_multRes)
	: m_max_hp{ t_max_hp },
	m_hp{ t_max_hp },
	m_corpse_name{ t_corpse_name },
	m_xp{ t_xp },
	m_resistanceModel{ t_flatRes, t_multRes }{}

Destructible& Destructible::setFlatResistance(const DamageType t_dmgType, const float t_value)
{
	m_resistanceModel.setFlatRes(t_dmgType, t_value);
	return *this;
}



Destructible& Destructible::setFlatResistance(const DmgArr& t_values)
{
	m_resistanceModel.setFlatRes(t_values);
	return *this;
}

Destructible& Destructible::setMultResistance(const DamageType t_dmgType, const float t_value)
{
	m_resistanceModel.setMultRes(t_dmgType, t_value);
	return *this;
}

Destructible& Destructible::setMultResistance(const DmgArr& t_values)
{
	m_resistanceModel.setMultRes(t_values);
	return *this;
}

float Destructible::takeDamage(Actor* t_owner, const DamageType t_damage_type, float t_damage) {
	t_damage = m_resistanceModel.reduceDamge(t_damage_type, t_damage);

	if (t_damage > 0) {
		m_hp -= t_damage;
		if (m_hp <= 0) {
			die(t_owner);
		}
	}
	else {
		t_damage = 0;
	}
	return t_damage;
}

void Destructible::die(Actor* t_owner)
{
	// Make the actor a corpse
	t_owner->m_char = '%';
	t_owner->m_color = TCODColor::darkRed;
	t_owner->m_name = m_corpse_name;
	t_owner->m_blocks = false;

	// Make sure corpses are drawn under living actors
	Engine::s_engine->sendToBack(t_owner);
}

float Destructible::heal(float t_amount)
{
	m_hp += t_amount;
	if (m_hp > m_max_hp)
	{
		t_amount -= m_hp - m_max_hp;
		m_hp = m_max_hp;
	}
	return t_amount;
}


MonsterDestructible::MonsterDestructible(const float t_max_hp, const char* t_corpse_name, const int t_xp, const DmgArr& t_flat_resistances, const DmgArr& t_mult_resistances)
	: Destructible(t_max_hp, t_corpse_name, t_xp, t_flat_resistances, t_mult_resistances) {}

void MonsterDestructible::die(Actor* t_owner)
{
	// Transform into corpse, can't move, can't be attacked, doesn't block
	Engine::messageFirstUpper(TCODColor::lightGrey, "%s is dead. You gain %d xp.", t_owner->m_name.c_str(), m_xp);
	Engine::s_engine->m_player->m_destructible->m_xp += m_xp;
	Destructible::die(t_owner);
}


PlayerDestructible::PlayerDestructible(const float t_max_hp, const char* t_corpse_name, const int t_xp, const DmgArr& t_flat_resistances, const DmgArr& t_mult_resistances)
	: Destructible(t_max_hp, t_corpse_name, t_xp, t_flat_resistances, t_mult_resistances) {}



void PlayerDestructible::die(Actor* t_owner)
{
	Engine::s_engine->m_gui->message(COL::PLAYER_MSG_COLOR, "You died!");
	Destructible::die(t_owner);
	Engine::s_engine->m_game_status = Engine::GAME_STATUS::DEFEAT;
}


