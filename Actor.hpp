#ifndef ACTOR_HPP
#define ACTOR_HPP
#include "libtcod/color.hpp"
#include "libtcod/console.hpp"

// Actor: anything other than a tile with a position and a char.

class Engine;

class Attacker;
class Destructible;
class Ai;
class Pickable;
class Container;

enum class ActorPreset
{
	PLAYER,
	ORC,
	TROLL
};


class Actor : public Persistent
{
public:
	class Builder
	{
	protected:
		std::unique_ptr<Actor> m_actor_wip;

	public:
		Builder();
		Builder& createActor(const ActorPreset t_preset, const int t_x, const int t_y);
		Builder& createActor(const int t_x, const int t_y, const char t_char, const char* t_name, const TCODColor& t_color, const bool t_blocks = true, const bool t_fov_only);

		template <typename T, typename T1, typename... Args>
		Builder& setAttacker(const T1 t_first, const Args... t_args) {
			m_actor_wip->m_attacker = std::make_unique<T>(t_first, t_args...);
			return *this;
		}

		template <typename T>
		Builder& setDestructible(const float t_max_hp, 
			const char* t_corpse_name, 
			const int t_xp, 
			const std::array<const float, (int)DamageType::TRUE>& t_flat_resistances = { 0.0f, 0.0f, 0.0f, 0.0f },
			const std::array<const float, (int)DamageType::TRUE>& t_mult_resistances = { 1.0f, 1.0f, 1.0f, 1.0f })
		{
			m_actor_wip->m_destructible = std::make_unique<T>(t_max_hp t_corpse_name, t_xp,t_flat_resistances,t_mult_resistances);
			return *this;
		}

		template <typename T>
		Builder& setAi()
		{
			m_actor_wip->m_ai = std::make_unique<T>();
			return *this;
		}

		template <typename T, typename T1, typename... Args>
		Builder& setPickable(const T1 t_first, const Args... t_args)
		{
			m_actor_wip->m_pickable = std::make_unique<T>(t_first, t_args...);
			m_actor_wip->m_blocks = false;
			return *this;
		}

		Builder& setContainer(const int t_size);
		Builder& setBlocks(const bool t_blocks);
		Builder& setFovOnly(const bool t_only);
		std::unique_ptr<Actor> build();
	};

	int m_x;
	int m_y;
	char m_char;
	std::string m_name;
	TCODColor m_color;
	bool m_proper_noun;						      // Whether it's name is a proper nown


	bool m_blocks;                                // Can it be walked over?
	bool m_fov_only;                              // Whether it is only seen when in fov or not
	std::unique_ptr<Attacker> m_attacker;         // Deals damage?
	std::unique_ptr<Destructible> m_destructible; // Can be damaged?
	std::unique_ptr<Ai> m_ai;                     // Self-updating?
	std::unique_ptr<Pickable> m_pickable;         // Can it be inside an inventory?
	std::unique_ptr<Container> m_container;       // Does it have an inventory itself?

	static Builder s_builder; // Use this guy to build new actors

public:
	Actor(const int t_x, const int t_y, const char t_char, const char* t_name, const TCODColor& t_color, const bool t_blocks = true, const bool t_fov_only = true, const bool t_proper_noun = false);
	~Actor();
	void update();
	void render() const;
	float getDistance(const int t_x, const int t_y) const;
	Actor& clear(const char t_replacement = ' ');
	void save(TCODZip& t_zip);
	void load(TCODZip& t_zip);
};
#endif