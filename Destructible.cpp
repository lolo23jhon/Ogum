#include <stdio.h>
#include "main.hpp"


Destructible::Destructible(
	const float t_max_hp,
	const char* t_corpse_name,
	const int t_xp,
	const std::array<const float, (int)DamageType::TRUE>& t_flat_resistances,
	const std::array<const float, (int)DamageType::TRUE>& t_mult_resistances)
	: m_max_hp{ t_max_hp },
	m_hp{ t_max_hp },
	m_corpse_name{ t_corpse_name },
	m_xp{ t_xp }
{
	for (int i{ 0 }; i < (int)DamageType::TRUE; i++)
	{
		m_flat_resistances.insert({ (DamageType)i, t_flat_resistances[i] });
		m_mult_resistances.insert({ (DamageType)i, t_mult_resistances[i] });
	}
}

Destructible& Destructible::setFlatResistance(const DamageType t_dmg_type, const float t_value)
{
	m_flat_resistances[t_dmg_type] = t_value;
	return *this;
}

Destructible& Destructible::setFlatResistance(const std::array<const float, (int)DamageType::TRUE>& t_flat_list)
{
	m_flat_resistances.clear();
	for (int i{ 0 }; i < (int)DamageType::TRUE; i++)
	{
		m_flat_resistances.insert({ (DamageType)i, t_flat_list[i] });
	}
	return *this;
}

Destructible& Destructible::setMultResistance(const DamageType t_dmg_type, const float t_mult)
{
	m_mult_resistances[t_dmg_type] = t_mult;
	return *this;
}

Destructible& Destructible::setMultResistance(const std::array<const float, (int)DamageType::TRUE>& t_mult_list)
{
	m_mult_resistances.clear();
	for (int i{ 0 }; i < (int)DamageType::TRUE; i++)
	{
		m_mult_resistances.insert({ (DamageType)i, t_mult_list[i] });
	}
	return *this;
}

float Destructible::takeDamage(Actor* t_owner, const DamageType t_damage_type, float t_damage)
{
	t_damage -= m_flat_resistances[t_dmg_type];
	t_damage *= m_mult_resistances[t_dmg_type];
	if (t_damage > 0)
	{
		m_hp -= t_damage;
		if (m_hp <= 0)
		{
			die(t_owner);
		}
	}
	else
	{
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


MonsterDestructible::MonsterDestructible(const float t_max_hp, const char* t_corpse_name, const int t_xp, const std::array<const float, (int)DamageType::TRUE>& t_flat_resistances, const std::array<const float, (int)DamageType::TRUE>& t_mult_resistances)
	: Destructible(t_max_hp, t_corpse_name,t_flat_resistances, t_mult_resistances){}

void MonsterDestructible::die(Actor* t_owner)
{
	// Transform into corpse, can't move, can't be attacked, doesn't block
	Engine::s_engine->m_gui->message(TCODColor::lightGrey, "%s is dead. You gain %d xp.", t_owner->m_name.c_str(), m_xp);
	Engine::s_engine->m_player->m_destructible->m_xp += m_xp;
	Destructible::die(t_owner);
}


PlayerDestructible::PlayerDestructible(const float t_max_hp, const char* t_corpse_name, const int t_xp, const std::array<const float, (int)DamageType::TRUE>& t_flat_resistances, const std::array<const float, (int)DamageType::TRUE>& t_mult_resistances)
	: Destructible(t_max_hp, t_corpse_name, t_flat_resistances, t_mult_resistances) {}


void PlayerDestructible::die(Actor* t_owner)
{
	Engine::s_engine->m_gui->message(COL::PLAYER_MSG_COLOR, "You died!");
	Destructible::die(t_owner);
	Engine::s_engine->m_game_status = Engine::GAME_STATUS::DEFEAT;
}


