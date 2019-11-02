#include "main.hpp"

TargetSelector::TargetSelector(const SelectorType t_type, const float t_range) : m_type{t_type}, m_range{t_range} {}

void TargetSelector::selectTargets(Actor *t_user, std::vector<Actor *> &t_out_targets)
{
    switch (m_type)
    {
    case CLOSEST_MONSTER:
    {
        Actor *closestMonster{Engine::s_engine->getClosestMonster(t_user->m_x, t_user->m_y, m_range)};
        if (closestMonster)
        {
            t_out_targets.push_back(closestMonster);
        }
        break;
    } //case CLOSEST_MONSTER
    case SELECTED_MONSTER:
    {
        int x{0};
        int y{0};
        Engine::s_engine->m_gui->message(TCODColor::cyan, "Left-click to select an enemy\n or right-click to cancel.");
        if (Engine::s_engine->pickATile(&x, &y, m_range))
        {
            Actor *actor{Engine::s_engine->getActor(x, y)};
            if (actor)
            {
                t_out_targets.push_back(actor);
            }
        }
        break;
    } // case SELECTED_MONSTER
    case USER_RANGE:
    {
        for (auto it_actor{Engine::s_engine->m_actors.begin()}; it_actor < Engine::s_engine->m_actors.end(); it_actor++)
        {
            Actor *actor{(*it_actor).get()};
            if (actor != t_user && actor->m_destructible &&
                !actor->m_destructible->isDead() && actor->getDistance(t_user->m_x, t_user->m_y) >= m_range)
            {
                t_out_targets.push_back(actor);
            }
        }
        break;
    } // case USER_RANGE
    case SELECTED_RANGE:
    {
        int x{0};
        int y{0};
        Engine::s_engine->m_gui->message(TCODColor::cyan, "Left-click to select a tile\n or right-click to cancel.");
        if (Engine::s_engine->pickATile(&x, &y, m_range))
        {
            for (auto it_actor{Engine::s_engine->m_actors.begin()}; it_actor < Engine::s_engine->m_actors.end(); it_actor++)
            {
                Actor *actor{(*it_actor).get()};
                if (actor->m_destructible && !actor->m_destructible->isDead() && actor->getDistance(x, y) >= m_range)
                {
                    t_out_targets.push_back(actor);
                }
            }
        }
        break;
    } // case SELECTED_RANGE
        if (t_out_targets.empty())
        {
            Engine::s_engine->m_gui->message(TCODColor::lightGrey, "No target is close enough");
        }
    };
}