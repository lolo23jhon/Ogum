#include "main.hpp"

///////////////////////////////////////////////////// BSPLISTENER::VISITNODE /////////////////////////////////////////////////////////////////

bool BspListener::visitNode(TCODBsp *t_node, void *t_userData)
{

    if (t_node->isLeaf())
    {
        bool withActors{(bool)t_userData};
        // Dig a room
        int w{m_map.m_rng->getInt(ROOM_MIN_SIZE, t_node->w - 2)};
        int h{m_map.m_rng->getInt(ROOM_MIN_SIZE, t_node->h - 2)};
        int x{m_map.m_rng->getInt(t_node->x + 1, t_node->x + t_node->w - w - 1)};
        int y{m_map.m_rng->getInt(t_node->y + 1, t_node->y + t_node->h - h - 1)};
        m_map.createRoom(m_room_num == 0, x, y, x + w - 1, y + h - 1, withActors);

        if (m_room_num != 0)
        {
            // Dig corridor
            m_map.dig(m_last_x, m_last_y, x + w / 2, m_last_y);
            m_map.dig(x + w / 2, m_last_y, x + w / 2, y + h / 2);
        }

        m_last_x = x + w / 2;
        m_last_y = y + h / 2;
        m_room_num++;
    }
    return true;
}

///////////////////////////////////////////////////// bsplistener::visitnode /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// STATICS /////////////////////////////////////////////////////////////////

static const int MAX_ROOM_ITEMS{2};

///////////////////////////////////////////////////// statics /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::MAP /////////////////////////////////////////////////////////////////

Map::Map(const int t_width, const int t_height) : m_width{t_width}, m_height{t_height}
{
    m_rng_seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}

///////////////////////////////////////////////////// map::map/////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::INIT /////////////////////////////////////////////////////////////////

void Map::init(const bool t_withActors)
{
    m_rng = std::make_unique<TCODRandom>(m_rng_seed, TCOD_RNG_CMWC);
    m_tiles = std::move(std::vector<Tile>(m_width * m_height, Tile()));
    m_tcod_map = std::make_unique<TCODMap>(m_width, m_height);
    TCODBsp bsp{0, 0, m_width, m_height};
    bsp.splitRecursive(m_rng.get(), 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
    BspListener listener{*this};
    bsp.traverseInvertedLevelOrder(&listener, (void *)t_withActors);
}

///////////////////////////////////////////////////// map::init /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::ISWALL /////////////////////////////////////////////////////////////////

bool Map::isWall(const int t_x, const int t_y) const
{
    return !m_tcod_map->isWalkable(t_x, t_y);
}

///////////////////////////////////////////////////// map::iswall /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::CANWALK /////////////////////////////////////////////////////////////////

bool Map::canWalk(const int t_x, const int t_y) const
{
    // Return false if it's a wall
    if (isWall(t_x, t_y))
    {
        return false;
    }

    // Check if there is an actor there. If so, return false.
    for (auto it_actor{Engine::s_engine->m_actors.begin()}; it_actor < Engine::s_engine->m_actors.end(); it_actor++)
    {
        if ((*it_actor)->m_blocks && (*it_actor)->m_x == t_x && (*it_actor)->m_y == t_y)
        {
            return false;
        }
    }
    // If none of the above is true, the character is free to walk there.
    return true;
}

///////////////////////////////////////////////////// map::canwalk /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::ISEXPLORED /////////////////////////////////////////////////////////////////

bool Map::isExplored(const int t_x, const int t_y) const
{
    return m_tiles[t_y * m_width + t_x].m_explored;
}

///////////////////////////////////////////////////// map::isexplored /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::ISINFOV /////////////////////////////////////////////////////////////////

bool Map::isInFov(const int t_x, const int t_y) const
{
    if (t_x < 0 || t_x >= m_width || t_y < 0 || t_y >= m_height)
    {
        return false;
    }

    if (m_tcod_map->isInFov(t_x, t_y))
    {
        m_tiles[t_y * m_width + t_x].m_explored = true;
        return true;
    }
    return false;
}

///////////////////////////////////////////////////// map::isinfov /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::COMPUTEFOV /////////////////////////////////////////////////////////////////

void Map::computeFov() const
{
    m_tcod_map->computeFov(Engine::s_engine->m_player->m_x, Engine::s_engine->m_player->m_y, Engine::s_engine->m_fovRadius);
}

///////////////////////////////////////////////////// map::computefov /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::RENDER /////////////////////////////////////////////////////////////////

void Map::render() const
{
    static const TCODColor dark_wall(47, 43, 39);
    static const TCODColor dark_ground(50, 50, 150);
    static const TCODColor light_wall(130, 110, 50);
    static const TCODColor light_ground(200, 180, 50);

    for (int x{0}; x < m_width; x++)
    {
        for (int y{0}; y < m_height; y++)
        {
            if (isInFov(x, y))
            {
                TCODConsole::root->setCharBackground(x, y, isWall(x, y) ? light_wall : light_ground);
            }
            else if (isExplored(x, y))
            {
                TCODConsole::root->setCharBackground(x, y, isWall(x, y) ? dark_wall : dark_ground);
            }
        }
    }
}

///////////////////////////////////////////////////// map::render /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::DIG /////////////////////////////////////////////////////////////////

void Map::dig(int t_x1, int t_y1, int t_x2, int t_y2)
{
    if (t_x1 > t_x2)
    {
        int temp{t_x2};
        t_x2 = t_x1;
        t_x1 = temp;
    }
    if (t_y1 > t_y2)
    {
        int temp{t_y2};
        t_y2 = t_y1;
        t_y1 = temp;
    }
    for (int tile_x{t_x1}; tile_x <= t_x2; tile_x++)
    {
        for (int tile_y{t_y1}; tile_y <= t_y2; tile_y++)
        {
            m_tcod_map->setProperties(tile_x, tile_y, true, true);
        }
    }
#if VISIBLE_MAP_GEN == 1
    render();
    TCODConsole::root->flush();
    TCODSystem::sleepMilli(50);
#endif
}

///////////////////////////////////////////////////// map::dig /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::CREATEROOM /////////////////////////////////////////////////////////////////

void Map::createRoom(const bool t_first, const int t_x1, const int t_y1, const int t_x2, const int t_y2, const bool t_withActors)
{
    dig(t_x1, t_y1, t_x2, t_y2);
    if (!t_withActors)
    {
        return;
    }

    if (t_first)
    {
        // Put the player in the first room
        Engine::s_engine->m_player->m_x = (t_x1 + t_x2) / 2;
        Engine::s_engine->m_player->m_y = (t_y1 + t_y2) / 2;
    }
    else
    {
        TCODRandom *rng{TCODRandom::getInstance()};

        int num_monsters{rng->getInt(0, MAX_ROOM_MONSTERS)};

        while (num_monsters > 0)
        {
            int x{rng->getInt(t_x1, t_x2)};
            int y{rng->getInt(t_y1, t_y2)};

            if (canWalk(x, y))
            {
                addMonster(x, y);
            }

            num_monsters--;
        }

        // Add items
        int numItems{rng->getInt(0, MAX_ROOM_ITEMS)};
        while (numItems > 0)
        {
            int x{rng->getInt(t_x1, t_x2)};
            int y{rng->getInt(t_y1, t_y2)};
            if (canWalk(x, y))
            {
                addItem(x, y);
            }
            numItems--;
        }
        // Place the stairs in the middle of the last room
        Engine::s_engine->m_stairs->m_x = (t_x1 + t_x2) / 2;
        Engine::s_engine->m_stairs->m_y = (t_y1 + t_y2) / 2;
    }
}

///////////////////////////////////////////////////// map::createroom /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::ADDMONSTER /////////////////////////////////////////////////////////////////

void Map::addMonster(const int t_x, const int t_y)
{
    // Get an rng instance
    TCODRandom *rng{TCODRandom::getInstance()};

    if (rng->getInt(0, 100) < 80)
    {
        // Create an orc 80% of the time
        Engine::s_engine->m_actors.emplace_back(Actor::s_builder.createActor(t_x, t_y, 'o', "orc", TCODColor::desaturatedGreen)
                                                    .setDesctrucible<MonsterDestructible>(10.0f, 0.0f, "dead orc", 35)
                                                    .setAttacker(3.0f)
                                                    .setAi<MonsterAi>()
                                                    .build());
    }
    else
    {
        // Create a troll 20% of the time
        Engine::s_engine->m_actors.emplace_back(Actor::s_builder.createActor(t_x, t_y, 'T', "troll", TCODColor::desaturatedGreen)
                                                    .setDesctrucible<MonsterDestructible>(16.0f, 1.0f, "troll carcass", 100)
                                                    .setAttacker(3.0f)
                                                    .setAi<MonsterAi>()
                                                    .build());
    }
}

///////////////////////////////////////////////////// map::addmonster /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// MAP::ADDITEM /////////////////////////////////////////////////////////////////

void Map::addItem(const int t_x, const int t_y)
{
    TCODRandom *rng{TCODRandom::getInstance()};
    int dice{rng->getInt(0, 100)};

    if (dice < 70) // 70%
    {
        // Add a health potion
        Engine::s_engine->m_actors.emplace_back(Actor::s_builder.createActor(t_x, t_y, '!', "health potion", TCODColor::violet)
                                                    .setPickable<Healer>(4.0f)
                                                    .build());
    }
    else if (dice < 70 + 10) // 10%
    {
        // Create a lighting scroll
        Engine::s_engine->m_actors.emplace_back(Actor::s_builder.createActor(t_x, t_y, '#', "scroll of lighting bolt", TCODColor::lightYellow)
                                                    .setPickable<LightingBolt>(5.0f, 20.0f)
                                                    .build());
    }
    else if (dice < 70 + 10 + 10) // 10%
    {
        // Create a fireball scroll
        Engine::s_engine->m_actors.emplace_back(Actor::s_builder.createActor(t_x, t_y, '#', "scroll of fire ball", TCODColor::lightYellow)
                                                    .setPickable<FireBall>(3.0f, 12.0f)
                                                    .build());
    }
    else // 10%
    {
        // Create a confusion scroll
        Engine::s_engine->m_actors.emplace_back(Actor::s_builder.createActor(t_x, t_y, '#', "scroll of confusion", TCODColor::lightYellow)
                                                    .setPickable<Confuser>(10, 8.0f)
                                                    .build());
    }
}

///////////////////////////////////////////////////// map::additem /////////////////////////////////////////////////////////////////
