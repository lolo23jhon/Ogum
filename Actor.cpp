#include "main.hpp"


Actor::Builder Actor::s_builder{};


Actor::Actor(const int t_x, const int t_y, const char t_char, const char* t_name, const TCODColor& t_color, const bool t_blocks, const bool t_fov_only, const bool t_proper_noun)
	: m_x{ t_x },
	m_y{ t_y },
	m_char{ t_char },
	m_name{ t_name },
	m_color{ t_color },
	m_blocks{ t_blocks },
	m_fov_only{ t_fov_only },
	m_proper_noun{ t_proper_noun },
	m_attacker{ nullptr },
	m_destructible{ nullptr },
	m_ai{ nullptr },
	m_pickable{ nullptr },
	m_container{ nullptr } {}



void Actor::render() const
{
	TCODConsole::root->setChar(m_x, m_y, m_char);
	TCODConsole::root->setCharForeground(m_x, m_y, m_color);
}

void Actor::update()
{
	if (m_ai)
	{
		m_ai->update(this);
	}
}


float Actor::getDistance(const int t_x, const int t_y) const
{
	return sqrtf((float)((m_x - t_x) * (m_x - t_x) + (m_y - t_y) * (m_y - t_y)));
}


Actor::Builder::Builder() {}


Actor::Builder& Actor::Builder::createActor( const int t_x, const int t_y, const ActorPreset t_preset)
{
	switch (t_preset)
	{
	case ActorPreset::PLAYER:
	{
		createActor(t_x, t_y, '@', "player", TCODColor::white, true, true)
			.setAttacker<MeleeAttacker>(DamageType::PHYSICAL, "2d2", "punched")
			.setDestructible<PlayerDestructible>(30.0f, "your corpse", 0)
			.setAi<PlayerAi>()
			.setContainer(26);

	} break; // case PLAYER
	case ActorPreset::ORC:
	{
		createActor(t_x, t_y, 'o', "orc", TCODColor::desaturatedGreen, true, true)
			.setAttacker<MeleeAttacker>(DamageType::PHYSICAL, "1d3", "scratched")
			.setDestructible<MonsterDestructible>(10.0f, "dead orc", 35)
			.setAi<MonsterAi>();

	} break; // case ORC
	case ActorPreset::TROLL:
	{
		createActor(t_x, t_y, 'T', "troll", TCODColor::desaturatedGreen, true, true)
			.setAttacker<MeleeAttacker>(DamageType::PHYSICAL, "1d4", "clubed")
			.setDestructible<MonsterDestructible>(20.0f, "troll carcass", 100)
			.setAi<MonsterAi>();

	} break; // case TROLL
	};

	return *this;
};


Actor::Builder& Actor::Builder::createActor(const int t_x, const int t_y, const char t_char, const char* t_name, const TCODColor& t_color, const bool t_blocks, const bool t_fov_only)
{
	m_actor_wip = std::make_unique<Actor>(t_x, t_y, t_char, t_name, t_color, t_blocks, t_fov_only);
	return *this;
}


Actor::Builder& Actor::Builder::setContainer(const int t_size)
{
	m_actor_wip->m_container = std::make_unique<Container>(t_size);
	return *this;
}


Actor::Builder& Actor::Builder::setBlocks(const bool t_blocks)
{
	m_actor_wip->m_blocks = t_blocks;
	return *this;
}


Actor::Builder& Actor::Builder::setFovOnly(const bool t_fov_only)
{
	m_actor_wip->m_fov_only = t_fov_only;
	return *this;
}


std::unique_ptr<Actor> Actor::Builder::construct()
{
	return std::move(m_actor_wip);
}

