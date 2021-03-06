#include "main.hpp"

Container::Container(const unsigned int t_size) : m_size{t_size} {}


Container::~Container() {}


bool Container::hasSpace()
{
    return (m_size == 0 || m_inventory.size() < m_size);
}


bool Container::add(std::unique_ptr<Actor> t_actor)
{
    if (!hasSpace())
    {
        return false; // Inventory full
    }

    m_inventory.emplace_back(std::move(t_actor));
    return true;
}


// Can't be called after the item has been moved, use to delete existeing items instead
void Container::remove(Actor *t_owner, Actor *t_item)
{
    if (t_owner->m_container)
    {
        // Look for the item in the inventory
        auto it_actor{std::find_if(
            m_inventory.begin(),
            m_inventory.end(),
            [&t_item](std::unique_ptr<Actor> &t_slot) { return t_slot.get() == t_item; })};

        // Was the item found?
        if (it_actor != m_inventory.end())
        {
            // Completely delete the item
            m_inventory.erase(it_actor);
        }
    }
}


// Removes the item from its cource inventory; to be used with Container::add()
std::unique_ptr<Actor> Container::move(Actor *t_owner, Actor *t_actor)
{
    if (t_owner->m_container)
    {
        // Look for the item in the owners inventory
        for (auto it_actor{t_owner->m_container->m_inventory.begin()}; it_actor < t_owner->m_container->m_inventory.end(); it_actor++)
        {
            // Match?
            if ((*it_actor).get() == t_actor)
            {
                // Make move iterator
                auto to_move(std::make_move_iterator(it_actor));

                // Move to vessel
                std::unique_ptr<Actor> actor_ptr{*to_move};

                // Clean the mess at the source
                t_owner->m_container->m_inventory.erase(it_actor);

                // Return respecting move semantics
                return std::move(actor_ptr);
            }
        }
    }

    // Something went wrong?
    return nullptr;
}

