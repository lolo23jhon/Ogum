#include "main.hpp"


bool Pickable::pick(Actor *t_owner, Actor *t_picker)
{
    // Check if container has space
    if (t_picker->m_container && t_picker->m_container->hasSpace())
    {
        // Look for myself in the engines "inventory"
        auto it_actor{std::find_if(Engine::s_engine->m_actors.begin(), Engine::s_engine->m_actors.end(), [&t_owner](std::unique_ptr<Actor> &t_actor) { return t_actor.get() == t_owner; })};

        // Did i find myself?
        if (it_actor != Engine::s_engine->m_actors.end())
        {
            // Make move iterator
            auto to_move{std::make_move_iterator(it_actor)};

            // Add myself to the picker's inventory
            t_picker->m_container->m_inventory.emplace_back(*to_move);

            // Clean my previous position at the engine's "inventory"
            Engine::s_engine->m_actors.erase(it_actor);

            // Item moved suceesfully
            return true;
        }
    }

    // The destination doesn't have enough space to store me
    return false;
}


bool Pickable::use(Actor *t_owner, Actor *t_carrier)
{
    // Does the wearer even have an inventory?
    if (t_carrier->m_container)
    {
        // Remove already handles memory thanks to smart pointers
        t_carrier->m_container->remove(t_carrier, t_owner);
        return true;
    }

    return false;
}

void Pickable::drop(Actor *t_owner, Actor *t_dropper)
{
    // Check if owner has container
    if (t_dropper->m_container)
    {
        // Look for the items unique_ptr in the droppers inventory
        auto it_item{std::find_if(
            t_dropper->m_container->m_inventory.begin(),
            t_dropper->m_container->m_inventory.end(),
            [this](std::unique_ptr<Actor> &t_item) { return t_item->m_pickable.get() == this; })};

        // Check if there was a match
        if (it_item != t_dropper->m_container->m_inventory.end())
        {
            // Change coordinates to the dropper's coordinates, else the item would appeaer where it was first picked
            t_owner->m_x = t_dropper->m_x;
            t_owner->m_y = t_dropper->m_y;

            // Make move iterator
            auto to_move{std::make_move_iterator(it_item)};

            // Get the item into the level's "inventory"
            Engine::s_engine->m_actors.emplace_back(*to_move);

            // Clean the pludered item in the dropper's inventory
            t_dropper->m_container->m_inventory.erase(it_item);

            // Display a message
            Engine::s_engine->m_gui->message(TCODColor::lightGrey, "%s drops a %s.", t_dropper->m_name.c_str(), t_owner->m_name.c_str());
        }
    }
}

Healer::Healer(const float t_amount) : m_amount{t_amount} {}


bool Healer::use(Actor *t_owner, Actor *t_carrier)
{
    // Can the carrier even be healed?
    if (t_carrier->m_destructible)
    {
        // Heal by the amount of the healer item
        float amountHealed{t_carrier->m_destructible->heal(m_amount)};

        if (amountHealed > 0)
        {
            return Pickable::use(t_owner, t_carrier);
        }
    }
    return false;
}


LightingBolt::LightingBolt(const float t_range, const float t_damage) : m_range{t_range}, m_damage{t_damage} {}


bool LightingBolt::use(Actor *t_owner, Actor *t_user)
{
    Actor *closestMonster{Engine::s_engine->getClosestMonster(t_user->m_x, t_user->m_y, m_range)};
    if (!closestMonster)
    {
        Engine::s_engine->m_gui->message(TCODColor::lightGrey, "No enemy is close enough to strike.");
        return false;
    }
    // Hit closest monster for damage
    Engine::s_engine->m_gui->message(TCODColor::lightBlue, "A lighting bolt strikes the %s with a loud thunder!\nThe damage is %g hit points.", closestMonster->m_name.c_str(), m_damage);
    closestMonster->m_destructible->takeDamage(closestMonster, DamageType::ELECTRIC,m_damage);

    return Pickable::use(t_owner, t_user);
}


FireBall::FireBall(const float t_range, const float t_damage) : LightingBolt(t_range, t_damage) {}


bool FireBall::use(Actor *t_owner, Actor *t_user)
{
    Engine::s_engine->m_gui->message(TCODColor::cyan, "Left click a target to shoot fireball.\nRight click to cancel.");
    int x{0};
    int y{0};

    // pickATile() returns false if selection is canceled
    if (!Engine::s_engine->pickATile(&x, &y))
    {
        return false;
    }

    // Burn all in range (including player)
    Engine::s_engine->m_gui->message(TCODColor::orange, "The firebal explodes, burning everything within %g tiles!", m_range);
    for (auto it_actor{Engine::s_engine->m_actors.begin()}; it_actor < Engine::s_engine->m_actors.end(); it_actor++)
    {
        if ((*it_actor)->m_destructible && !(*it_actor)->m_destructible->isDead() && (*it_actor)->getDistance(x, y) <= m_range)
        {
            Engine::s_engine->m_gui->message(TCODColor::orange, "The %s gets burned for %g hit points.", (*it_actor)->m_name.c_str(), m_damage);
			(*it_actor)->m_destructible->takeDamage((*it_actor).get(), DamageType::HEAT, m_damage);
        }
    }
    return Pickable::use(t_owner, t_user);
}


Confuser::Confuser(const int t_num_turns, const float t_range) : m_num_turns{t_num_turns}, m_range{t_range} {}


bool Confuser::use(Actor *t_owner, Actor *t_user)
{
    Engine::s_engine->m_gui->message(TCODColor::cyan, "Left-click an enemy to confuse it,'n or right-click to cancel.");
    int x{0};
    int y{0};
    if (!Engine::s_engine->pickATile(&x, &y, m_range))
    {
        return false;
    }

    Actor *actor{Engine::s_engine->getActor(x, y)};
    if (!actor)
    {
        return false;
    }

    // Confuse the fpr <m_num_turns> turns
    std::unique_ptr<ConfusedMonsterAi> confuseAi{std::make_unique<ConfusedMonsterAi>(m_num_turns, std::move(actor->m_ai))};
    actor->m_ai = std::move(confuseAi);

    // Display use message and destroy the scroll
    Engine::s_engine->m_gui->message(TCODColor::lightGreen, "The eyes of the %s look vacant, \nas it starts to stumble around!", actor->m_name.c_str());
    return Pickable::use(t_owner, t_user);
}

