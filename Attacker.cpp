#include "main.hpp"

///////////////////////////////////////////////////// CONSTRUCTOR /////////////////////////////////////////////////////////////////

Attacker::Attacker(const float t_power) : m_power{t_power} {}

///////////////////////////////////////////////////// constructor /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ATTACK /////////////////////////////////////////////////////////////////

void Attacker::attack(Actor *t_owner, Actor *t_target)
{
    if (t_target->m_destructible && !t_target->m_destructible->isDead())
    {
        TCODRandom *rng{TCODRandom::getInstance()};
        float dmg_roll{rng->diceRoll(m_dice)};

        if (m_power - t_target->m_destructible->m_defense > 0)
        {
            Engine::s_engine->m_gui->message(t_owner == Engine::s_engine->m_player ? TCODColor::red : TCODColor::lightGrey,
                                             "%s attacks %s for %g hit point%s.",
                                             t_owner->m_name.c_str(),
                                             t_target->m_name.c_str(),
                                             m_power - t_target->m_destructible->m_defense,
                                             utl::isOne(m_power - t_target->m_destructible->m_defense) ? "" : "s");
        }
        else
        {
            Engine::s_engine->m_gui->message(TCODColor::lightGrey, "%s attacks %s but it has no effect!", t_owner->m_name.c_str(), t_target->m_name.c_str());
        }
        t_target->m_destructible->takeDamage(t_target, m_power);
    }
    else
    {
        Engine::s_engine->m_gui->message(TCODColor::lightGrey, "%s attacks %s in vain.", t_owner->m_name.c_str(), t_target->m_name.c_str());
    }
}

///////////////////////////////////////////////////// attack /////////////////////////////////////////////////////////////////
