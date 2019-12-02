#include "main.hpp"




Attacker::Attacker(const DamageType t_dmgType, const TCOD_dice_t t_dice, const char* t_verb) : m_attack{t_dmgType,t_dice,t_verb} {}


void Attacker::attack(Actor* t_owner, Actor* t_target)
{
	// Check if the target can even be attacked and if it's still alive
	if (t_target->m_destructible && !t_target->m_destructible->isDead())
	{
		const TCODColor& msg_color{ Engine::s_engine->m_player == t_target ? COL::PLAYER_MSG_COLOR : COL::NORMAL_MSG_COLOR };

		auto rng = Engine::s_rng;
		bool hit{ true };

		// Did the attack land?
		if (hit) {

			// Roll for damage (from the attack, without any other calculation)
			float dmg{ (float)m_attack.rollDmg() };

			// Damage calcuation
			dmg = t_target->m_destructible->takeDamage(t_target, m_attack.dmgType(), dmg);

			if (dmg > 0) {

				// Hit and damage
				Engine::messageFirstUpper(msg_color,
					"%s%s %s %s%s for %d hit points.",
					t_owner->m_proper_noun ? "" : "the ",
					t_owner->m_name.c_str(),
					m_attack.verb().c_str(),
					t_target->m_proper_noun ? "" : "the ",
					t_target->m_name.c_str(),
					dmg);
			}
			else
			{
				// Hit, but no damage
				Engine::messageFirstUpper(msg_color,
					"%s%s's attack had no effect on %s%s.",
					t_owner->m_proper_noun ? "" : "the ",
					t_owner->m_name.c_str(),
					t_target->m_proper_noun ? "" : "the ",
					t_target->m_name.c_str());
			}
		}
		else
		{
			// Dodge 
			Engine::messageFirstUpper(msg_color,
				"%s%s dodged %s%s's attack.",
				t_target->m_proper_noun ? "" : "the ",
				t_target->m_name.c_str(),
				t_owner->m_proper_noun ? "" : "the ",
				t_owner->m_name.c_str());
		}
	}

}