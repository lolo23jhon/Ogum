#include "main.hpp"

///////////////////////////////////////////////////// HEALTHEFFECT::HEALTHEFFECT /////////////////////////////////////////////////////////////////

HealthEffect::HealthEffect(const float t_amount, const char *t_message, const TCODColor &t_msg_color)
    : m_amount{t_amount}, m_message{t_message}, m_msg_color{t_msg_color} {}

///////////////////////////////////////////////////// healtheffect::healtheffect /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// HEALTHEFFECT::APPLYTO /////////////////////////////////////////////////////////////////

bool HealthEffect::applyTo(Actor *t_actor)
{
    if (!t_actor->m_destructible)
    {
        return false;
    }

    if (m_amount > 0)
    {
        float pointsHealed{t_actor->m_destructible->heal(m_amount)};
        if (pointsHealed > 0)
        {
            if (!m_message.empty())
            {
                // Must always use a string with a %s and a %d
                Engine::s_engine->m_gui->message(m_msg_color, m_message.c_str(), t_actor->m_name.c_str(), pointsHealed);
            }
        }
        else
        {
            if (!m_message.empty() && -m_amount - t_actor->m_destructible->m_defense > 0)
            {
            }
        }
    }
}

///////////////////////////////////////////////////// healtheffect::applyto /////////////////////////////////////////////////////////////////
