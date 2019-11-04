#include <math.h>
#include "main.hpp"

///////////////////////////////////////////////////// PLAYERAI::S_MOVEMENT_KEYS /////////////////////////////////////////////////////////////////

const std::unordered_map<TCOD_keycode_t, std::pair<int, int>> PlayerAi::s_movement_keys{
    // Numpad
    {TCODK_KP8, {0, -1}},  // N
    {TCODK_KP7, {-1, -1}}, // NW
    {TCODK_KP9, {1, -1}},  // NE
    {TCODK_KP2, {0, 1}},   // S
    {TCODK_KP1, {-1, 1}},  // SW
    {TCODK_KP3, {1, 1}},   // SE
    {TCODK_KP4, {-1, 0}},  // W
    {TCODK_KP6, {1, 0}},   // E

    // Arrowkeys
    {TCODK_UP, {0, -1}},   // N
    {TCODK_DOWN, {0, 1}},  // S
    {TCODK_LEFT, {-1, 0}}, // W
    {TCODK_RIGHT, {1, 0}}  // E

};

///////////////////////////////////////////////////// playerai::s_movement_keys /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// PLAYERAI::S_MOVEMENT_CHARS /////////////////////////////////////////////////////////////////

const std::unordered_map<char, std::pair<int, int>> PlayerAi::s_movement_chars{
    {'k', {0, -1}},  // N
    {'y', {-1, -1}}, // NW
    {'u', {1, -1}},  // NE
    {'j', {0, 1}},   // S
    {'b', {-1, 1}},  // SW
    {'n', {1, 1}},   // SE
    {'h', {-1, 0}},  // W
    {'l', {1, 0}}    // E
};

///////////////////////////////////////////////////// s_movement_chars /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// NON-MEMBER CONSTANTS /////////////////////////////////////////////////////////////////

const int LEVEL_UP_BASE{200};
const int LEVEL_UP_FACTOR{150};

///////////////////////////////////////////////////// non-member constants /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// PLAYERAI::PLAYERAI /////////////////////////////////////////////////////////////////

PlayerAi::PlayerAi() : m_xp_lvl{1} {}

///////////////////////////////////////////////////// playerai::playerai /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// PLAYERAI::GETNEXTLEVEL /////////////////////////////////////////////////////////////////

int PlayerAi::getNextLevel()
{
    return LEVEL_UP_BASE + m_xp_lvl * LEVEL_UP_FACTOR;
}

///////////////////////////////////////////////////// playerai::getnextlevel /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// PLAYERAI::UPDATE /////////////////////////////////////////////////////////////////

void PlayerAi::update(Actor *t_owner)
{
    // Check xp and level up the player
    int levelUpXp{getNextLevel()};
    if (t_owner->m_destructible->m_xp >= levelUpXp)
    {
        m_xp_lvl++;
        t_owner->m_destructible->m_xp -= levelUpXp;
        Engine::s_engine->m_gui->message(TCODColor::yellow, "Something within you grew stronger. You've reached level %d.", m_xp_lvl);
        Engine::s_engine->m_gui->m_menu.clear();
        Engine::s_engine->m_gui->m_menu.addItem(Menu::MenuItemCode::CONSTITUTION, "Constitution (+20 HP)");
        Engine::s_engine->m_gui->m_menu.addItem(Menu::MenuItemCode::STRENGTH, "Strength (+1 ATK)");
        Engine::s_engine->m_gui->m_menu.addItem(Menu::MenuItemCode::AGILITY, "Agility (+1 DEF)");
        Menu::MenuItemCode menuItem{Engine::s_engine->m_gui->m_menu.pick(Menu::DisplayMode::PAUSE)};

        switch (menuItem)
        {
        case Menu::MenuItemCode::CONSTITUTION:
           // t_owner->m_destructible->m_max_hp += 20;
           // t_owner->m_destructible->m_hp += 20;
            break;
        case Menu::MenuItemCode::STRENGTH:
           // t_owner->m_attacker->m_power += 1;
            break;
        case Menu::MenuItemCode::AGILITY:
           // t_owner->m_destructible->m_defense += 1;
            break;
        default:
            break;
        };
    }

    if (t_owner->m_destructible && t_owner->m_destructible->isDead())
    {
        return;
    }

    const std::pair<int, int> *it_dmov{nullptr};
    bool move{false};
    switch (Engine::s_engine->m_last_key.vk)
    {
    case TCODK_UP:
    case TCODK_DOWN:
    case TCODK_LEFT:
    case TCODK_RIGHT:
    case TCODK_KP8:
    case TCODK_KP7:
    case TCODK_KP9:
    case TCODK_KP2:
    case TCODK_KP1:
    case TCODK_KP3:
    case TCODK_KP4:
    case TCODK_KP6:
    {
        it_dmov = &s_movement_keys.at(Engine::s_engine->m_last_key.vk);
        move = true;
    }
    break;
    case TCODK_KP5:
        wait();
        break;
    case TCODK_CHAR:
        switch (Engine::s_engine->m_last_key.c)
        {
        case 'k':
        case 'y':
        case 'u':
        case 'j':
        case 'b':
        case 'n':
        case 'h':
        case 'l':
        {
            it_dmov = &s_movement_chars.at(Engine::s_engine->m_last_key.c);
            move = true;
            break;
        }
        default:
            handleActionKey(t_owner, Engine::s_engine->m_last_key.c, Engine::s_engine->m_last_key.shift);
            break;
        };
    };

    if (move)
    {
        Engine::s_engine->m_game_status = Engine::GAME_STATUS::NEW_TURN;

        if (moveOrAttack(t_owner, t_owner->m_x + it_dmov->first, t_owner->m_y + it_dmov->second))
        {
            Engine::s_engine->m_map->computeFov();
        }
    }
}

///////////////////////////////////////////////////// playerai::update /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// PLAYERAI::HANDLEACTIONKEY /////////////////////////////////////////////////////////////////

void PlayerAi::handleActionKey(Actor *t_owner, const int t_ascii, const bool t_shift)
{
    switch (t_ascii)
    {
    case 'g': // Pickup item
    {
        bool found{false};
        // Loop through the actors in the engine's "inventory"
        for (auto it_actor{Engine::s_engine->m_actors.begin()}; it_actor < Engine::s_engine->m_actors.end(); it_actor++)
        {
            // Is the actor pickable and the position matches?

            if ((*it_actor)->m_pickable && (*it_actor)->m_x == t_owner->m_x && (*it_actor)->m_y == t_owner->m_y)
            {
                const char *actor_name{(*it_actor)->m_name.c_str()};
                // Try to pick it up
                if ((*it_actor)->m_pickable->pick((*it_actor).get(), t_owner))
                {
                    found = true;
                    Engine::s_engine->m_gui->message(TCODColor::lightGrey, "You pick up the %s.", actor_name);
                    break; // For loop
                }
                else if (!found) // Found an item but inventory is full
                {
                    found = true;
                    Engine::s_engine->m_gui->message(TCODColor ::red, "Your inventory is full.");
                }
            }
        }

        if (!found) // No item found at all
        {
            Engine::s_engine->m_gui->message(TCODColor::lightGrey, "There's nothing here you can pickup.");
        }
        Engine::s_engine->m_game_status = Engine::GAME_STATUS::NEW_TURN;
        break;
    } // case: 'g'
    case '.':
    {
        wait();
        break;
    } // case '.'
    case 'i':
    {
        Actor *item{chooseFromInventory(t_owner)};

        if (item)
        {
            item->m_pickable->use(item, t_owner);
            Engine::s_engine->m_game_status = Engine::GAME_STATUS::NEW_TURN; // Using (or trying to) use an item consumes a turn
        }
        break;
    } // case 'i'
    case 'd':
    {
        Actor *item{chooseFromInventory(t_owner)};

        if (item)
        {
            item->m_pickable->drop(item, t_owner);
            Engine::s_engine->m_game_status = Engine::GAME_STATUS::NEW_TURN;
        }
        break;
    } // case 'd'
    case '<':
    {
        // Check if shift was pressed, to mean > instead of <
        if (!t_shift)
        {
            break;
        }
        // Check if player is over the stairs
        if (Engine::s_engine->m_stairs->m_x == t_owner->m_x && Engine::s_engine->m_stairs->m_y == t_owner->m_y)
        {
            Engine::s_engine->nextLevel();
        }
        else
        {
            Engine::s_engine->m_gui->message(TCODColor::lightGrey, "There are no stairs here.");
        }
        break;
    } // case '<'
    };
}

///////////////////////////////////////////////////// playerai::handleactionkey /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// PLAYERAI::CHOOSEFROMINVENTORY /////////////////////////////////////////////////////////////////

Actor *PlayerAi::chooseFromInventory(Actor *t_owner)
{
    static const int INVENTORY_WIDTH{50};
    static const int INVENTORY_HEIGHT{28};
    static TCODConsole con{INVENTORY_WIDTH, INVENTORY_HEIGHT};

    // Display inventory frame
    con.setDefaultForeground(TCODColor(200, 180, 50));
    con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");

    // Display the items with their keyboard shorcut
    con.setDefaultForeground(TCODColor::white);
    int shorcut{'a'};
    int y{1};
    for (auto it_item{t_owner->m_container->m_inventory.begin()}; it_item < t_owner->m_container->m_inventory.end(); it_item++)
    {
        con.printf(2, y, "(%c) %s", shorcut, (*it_item)->m_name.c_str());
        y++;
        shorcut++;
    }

    // blit the inventory console on the root console and flush it
    TCODConsole::root->blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, TCODConsole::root, Engine::s_engine->m_screen_w / 2 - INVENTORY_WIDTH / 2, Engine::s_engine->m_screen_h - INVENTORY_HEIGHT / 2);
    TCODConsole::flush();

    // Wait for the player to press a key
    TCOD_key_t key;
    TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
    if (key.vk == TCODK_CHAR)
    {
        // Subtract the entered letter with the starting symbol (a) to get the correct index
         int itemIndex{key.c - 'a'};
        if (itemIndex >= 0 && itemIndex < t_owner->m_container->m_inventory.size())
        {
            return t_owner->m_container->m_inventory[itemIndex].get();
        }
    }
    return nullptr;
}

///////////////////////////////////////////////////// playerai::choosefrominventory /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// PLAYERAI::MOVEORATTACK /////////////////////////////////////////////////////////////////

bool PlayerAi::moveOrAttack(Actor *t_owner, const int t_target_x, const int t_target_y)
{
    // Trying to move into/hit a wall?
    if (Engine::s_engine->m_map->isWall(t_target_x, t_target_y))
    {
        // No movement, so false
        return false;
    }

    // Iterate through all the actors in the map
    for (auto it_actor{Engine::s_engine->m_actors.begin()}; it_actor < Engine::s_engine->m_actors.end(); it_actor++)
    {
        // Check if it's desctructible, if it's alive, and if its position matches
        if ((*it_actor)->m_destructible &&
            !(*it_actor)->m_destructible->isDead() &&
            (*it_actor)->m_x == t_target_x &&
            (*it_actor)->m_y == t_target_y)
        {
            t_owner->m_attacker->attack(t_owner, (*it_actor).get());
            // No movement, so false
            return false;
        }
    }
    // Look for corpses
    for (auto it_actor{Engine::s_engine->m_actors.begin()}; it_actor < Engine::s_engine->m_actors.end(); it_actor++)
    {
        bool corpseOrItem{(*it_actor)->m_destructible && (*it_actor)->m_destructible->isDead() || ((*it_actor)->m_pickable)};

        if (corpseOrItem &&
            (*it_actor)->m_x == t_target_x &&
            (*it_actor)->m_y == t_target_y)
        {
            Engine::s_engine->m_gui->message(TCODColor::lightGrey,
                                             "There is a%s %s here.",
                                             utils::startsVowel((*it_actor)->m_name) ? "n" : "",
                                             (*it_actor)->m_name.c_str());
        }
    }
    t_owner->m_x = t_target_x;
    t_owner->m_y = t_target_y;
    return true;
}

///////////////////////////////////////////////////// playerai::moveorattack /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// PLAYERAI::WAIT /////////////////////////////////////////////////////////////////

void PlayerAi::wait(const unsigned int t_num_turns)
{
    for (unsigned int i{0}; i < t_num_turns; i++)
    {
        Engine::s_engine->m_game_status = Engine::GAME_STATUS::NEW_TURN;
    }
}

///////////////////////////////////////////////////// playerai::wait /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MONSTERAI::S_TRACKING_TURNS /////////////////////////////////////////////////////////////////

// Number of turns the monster chases after losing sight
const int MonsterAi::S_TRACKING_TURNS{3};

///////////////////////////////////////////////////// monsterai::s_tracking_turns /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MONSTERAI::CONSTRUCTOR /////////////////////////////////////////////////////////////////

MonsterAi::MonsterAi() : m_move_count{0} {}

///////////////////////////////////////////////////// monsterai::constructor /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MONSTERAI::UPDATE /////////////////////////////////////////////////////////////////

void MonsterAi::update(Actor *t_owner)
{
    // Corpses can't move (for the most part)
    if (t_owner->m_destructible &&
        t_owner->m_destructible->isDead())
    {
        return;
    }

    // Using player's fov here
    if (Engine::s_engine->m_map->isInFov(t_owner->m_x, t_owner->m_y))
    {
        // We can see the player, move towards it
        m_move_count = S_TRACKING_TURNS;
    }
    else
    {
        m_move_count--;
    }

    if (m_move_count > 0)
    {
        moveOrAttack(t_owner, Engine::s_engine->m_player->m_x, Engine::s_engine->m_player->m_y);
    }
}

///////////////////////////////////////////////////// monsterai::update /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MONSTERAI::MOVEORATTACK /////////////////////////////////////////////////////////////////

void MonsterAi::moveOrAttack(Actor *t_owner, const int t_target_x, const int t_target_y)
{
    int dx{t_target_x - t_owner->m_x};
    int dy{t_target_y - t_owner->m_y};
    int step_dx{dx > 0 ? 1 : -1};
    int step_dy{dy > 0 ? 1 : -1};
    float distance{sqrtf((float)(dx * dx + dy * dy))};

    if (distance >= 2)
    {
        // Normalization of the vector monster -> player
        dx = (int)(round(dx / distance));
        dy = (int)(round(dy / distance));

        if (Engine::s_engine->m_map->canWalk(t_owner->m_x + dx, t_owner->m_y + dy))
        {
            t_owner->m_x += dx;
            t_owner->m_y += dy;
        }
        else if (Engine::s_engine->m_map->canWalk(t_owner->m_x + step_dx, t_owner->m_y + step_dy))
        {
            t_owner->m_x += step_dx;
            t_owner->m_y += step_dy;
        }
        else if (Engine::s_engine->m_map->canWalk(t_owner->m_x + step_dx, t_owner->m_y))
        {

            t_owner->m_x += step_dx;
        }
        else if (Engine::s_engine->m_map->canWalk(t_owner->m_x, t_owner->m_y + step_dy))
        {
            t_owner->m_y += step_dy;
        }
    }
    else if (t_owner->m_attacker)
    {
        t_owner->m_attacker->attack(t_owner, Engine::s_engine->m_player);
    }
}

///////////////////////////////////////////////////// monsterai::moveorarrack /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// CONFUSEDMONSTER::CONFUSEDMONSTER /////////////////////////////////////////////////////////////////

ConfusedMonsterAi::ConfusedMonsterAi(const int t_num_turns, std::unique_ptr<Ai> t_old_ai) : m_num_turns{t_num_turns}, m_old_ai{std::move(t_old_ai)} {}

///////////////////////////////////////////////////// confusedmonster::confusedmonster /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// CONFUSEDMONSTER::UPDATE /////////////////////////////////////////////////////////////////

void ConfusedMonsterAi::update(Actor *t_owner)
{
    TCODRandom *rng{TCODRandom::getInstance()};
    int dx{rng->getInt(-1, 1)};
    int dy{rng->getInt(-1, 1)};

    if (dx != 0 || dy != 0)
    {
        int dest_x{t_owner->m_x + dx};
        int dest_y{t_owner->m_y + dy};
        if (Engine::s_engine->m_map->canWalk(dest_x, dest_y))
        {
            t_owner->m_x = dest_x;
            t_owner->m_y = dest_y;
        }
        else
        {
            Actor *actor{Engine::s_engine->getActor(dest_x, dest_y)};
            if (actor)
            {
                t_owner->m_attacker->attack(t_owner, actor);
            }
        }
    }
    m_num_turns--;
    if (m_num_turns == 0)
    {
        t_owner->m_ai = std::move(m_old_ai);
    }
}

///////////////////////////////////////////////////// confusedmonster::update /////////////////////////////////////////////////////////////////
