#include "main.hpp"


//================================================== MAP ==================================================//

void Map::load(TCODZip &t_zip)
{
    // Recuperate seed
    m_rng_seed = t_zip.getInt();
    // Generate map with no actors
    init(false);

    // Restore player character's memory of the map
    for (int i{0}; i < m_width * m_height; i++)
    {
        m_tiles[i].m_explored = t_zip.getInt();
    }
}

void Map::save(TCODZip &t_zip)
{
    t_zip.putInt(m_rng_seed);
    for (int i{0}; i < m_width * m_height; i++)
    {
        t_zip.putInt(m_tiles[i].m_explored);
    }
}

//================================================== map ==================================================//
//================================================== ACTOR ==================================================//

void Actor::load(TCODZip &t_zip)
{
    // Regular members
    m_x = t_zip.getInt();
    m_y = t_zip.getInt();
    m_char = t_zip.getInt();
    m_color = t_zip.getColor();
    m_name = t_zip.getString();
    m_blocks = t_zip.getInt();
    m_fov_only = t_zip.getInt();

    // Components' booleans
    bool hasAttacker{static_cast<bool>(t_zip.getInt())};
    bool hasDestructible{static_cast<bool>(t_zip.getInt())};
    bool hasAi{static_cast<bool>(t_zip.getInt())};
    bool hasPickable{static_cast<bool>(t_zip.getInt())};
    bool hasContainer{static_cast<bool>(t_zip.getInt())};

    // Components themselves
    if (hasAttacker)
    {
        m_attacker = std::make_unique<Attacker>(0.0f);
        m_attacker->load(t_zip);
    }
    if (hasDestructible)
    {
        m_destructible = Destructible::create(t_zip);
    }
    if (hasAi)
    {
        m_ai = Ai::create(t_zip);
    }
    if (hasPickable)
    {
        m_pickable = Pickable::create(t_zip);
    }
    if (hasContainer)
    {
        m_container = std::make_unique<Container>(0);
        m_container->load(t_zip);
    }
}

void Actor::save(TCODZip &t_zip)
{
    // Regular members
    t_zip.putInt(m_x);
    t_zip.putInt(m_y);
    t_zip.putInt(m_char);
    t_zip.putColor(&m_color);
    t_zip.putString(m_name.c_str());
    t_zip.putInt(m_blocks);
    t_zip.putInt(m_fov_only);

    // Boolean of components
    t_zip.putInt(m_attacker != nullptr);
    t_zip.putInt(m_destructible != nullptr);
    t_zip.putInt(m_ai != nullptr);
    t_zip.putInt(m_pickable != nullptr);
    t_zip.putInt(m_container != nullptr);

    // Components themselves
    if (m_attacker)
    {
        m_attacker->save(t_zip);
    }
    if (m_destructible)
    {
        m_destructible->save(t_zip);
    }
    if (m_ai)
    {
        m_ai->save(t_zip);
    }
    if (m_pickable)
    {
        m_pickable->save(t_zip);
    }
    if (m_container)
    {
        m_container->save(t_zip);
    }
}

//================================================== actor ==================================================//
//================================================== CONTAINER ==================================================//

void Container::load(TCODZip &t_zip)
{
    m_size = t_zip.getInt();
    int num_actors = t_zip.getInt();
    while (num_actors > 0)
    {
        m_inventory.emplace_back(std::make_unique<Actor>(0, 0, 0, "", TCODColor::white));
        m_inventory.back()->load(t_zip);
        num_actors--;
    }
}

void Container::save(TCODZip &t_zip)
{
    t_zip.putInt(m_size);
    t_zip.putInt(m_inventory.size());
    for (auto it_actor{m_inventory.begin()}; it_actor < m_inventory.end(); it_actor++)
    {
        (*it_actor)->save(t_zip);
    }
}

//================================================== container ==================================================//
//================================================== DESTRUCTIBLE ==================================================//

void Destructible::load(TCODZip &t_zip)
{
    m_max_hp = t_zip.getFloat();
    m_hp = t_zip.getFloat();
    m_defense = t_zip.getFloat();
    m_corpse_name = t_zip.getString();
    m_xp = t_zip.getInt();
}

void Destructible::save(TCODZip &t_zip)
{
    t_zip.putFloat(m_max_hp);
    t_zip.putFloat(m_hp);
    t_zip.putFloat(m_defense);
    t_zip.putString(m_corpse_name.c_str());
    t_zip.putInt(m_xp);
}

void PlayerDestructible::save(TCODZip &t_zip)
{
    t_zip.putInt(DestructibleType::PLAYER);
    Destructible::save(t_zip);
}

void MonsterDestructible::save(TCODZip &t_zip)
{
    t_zip.putInt(DestructibleType::MONSTER);
    Destructible::save(t_zip);
}

std::unique_ptr<Destructible> Destructible::create(TCODZip &t_zip)
{
    DestructibleType type{(DestructibleType)t_zip.getInt()};
    std::unique_ptr<Destructible> destructible{nullptr};
    switch (type)
    {
    case DestructibleType::MONSTER:
        destructible = std::make_unique<MonsterDestructible>(0.0f, 0.0f, "", 0);
        break;
    case DestructibleType::PLAYER:
        destructible = std::make_unique<PlayerDestructible>(0.0f, 0.0f, "");
        break;
    };
    destructible->load(t_zip);
    return std::move(destructible);
}

//================================================== destructible ==================================================//
//================================================== ATTACKER ==================================================//

void Attacker::load(TCODZip &t_zip)
{
    m_power = t_zip.getFloat();
}

void Attacker::save(TCODZip &t_zip)
{
    t_zip.putFloat(m_power);
}

//================================================== attacker ==================================================//
//================================================== AI ==================================================//

void MonsterAi::load(TCODZip &t_zip)
{
    m_move_count = t_zip.getInt();
}
void MonsterAi::save(TCODZip &t_zip)
{
    t_zip.putInt(static_cast<int>(AiType::MONSTER));
    t_zip.putInt(m_move_count);
}

void ConfusedMonsterAi::load(TCODZip &t_zip)
{
    m_num_turns = t_zip.getInt();
    m_old_ai = Ai::create(t_zip);
}
void ConfusedMonsterAi::save(TCODZip &t_zip)
{
    t_zip.putInt(static_cast<int>(AiType::CONFUSED_MONSTER));
    t_zip.putInt(m_num_turns);
    m_old_ai->save(t_zip);
}

void PlayerAi::load(TCODZip &t_zip)
{
}
void PlayerAi::save(TCODZip &t_zip)
{
    t_zip.putInt(static_cast<int>(AiType::PLAYER));
}

std::unique_ptr<Ai> Ai::create(TCODZip &t_zip)
{
    AiType type{(AiType)t_zip.getInt()};
    std::unique_ptr<Ai> ai{nullptr};
    switch (type)
    {
    case AiType::PLAYER:
        ai = std::make_unique<PlayerAi>();
        break;
    case AiType::MONSTER:
        ai = std::make_unique<MonsterAi>();
        break;
    case AiType::CONFUSED_MONSTER:
        ai = std::make_unique<ConfusedMonsterAi>(0, nullptr);
        break;
    };
    ai->load(t_zip);
    return std::move(ai);
}

//================================================== ai ==================================================//
//================================================== PICKABLE ==================================================//

void Healer::load(TCODZip &t_zip)
{
    m_amount = t_zip.getFloat();
}
void Healer::save(TCODZip &t_zip)
{
    t_zip.putInt(static_cast<int>(PickableType::HEALER));
    t_zip.putFloat(m_amount);
}

void LightingBolt::load(TCODZip &t_zip)
{
    m_range = t_zip.getFloat();
    m_damage = t_zip.getFloat();
}
void LightingBolt::save(TCODZip &t_zip)
{
    t_zip.putInt(static_cast<int>(PickableType::LIGHTING_BOLT));
    t_zip.putFloat(m_range);
    t_zip.putFloat(m_damage);
}

void Confuser::load(TCODZip &t_zip)
{
    m_num_turns = t_zip.getInt();
    m_range = t_zip.getFloat();
}
void Confuser::save(TCODZip &t_zip)
{
    t_zip.putInt(static_cast<int>(PickableType::CONFUSER));
    t_zip.putInt(m_num_turns);
    t_zip.putFloat(m_range);
}

void FireBall::save(TCODZip &t_zip)
{
    t_zip.putInt(static_cast<int>(PickableType::FIREBALL));
    t_zip.putFloat(m_range);
    t_zip.putFloat(m_damage);
}

std::unique_ptr<Pickable> Pickable::create(TCODZip &t_zip)
{
    PickableType type{(PickableType)t_zip.getInt()};
    std::unique_ptr<Pickable> pickable{nullptr};
    switch (type)
    {
    case PickableType::HEALER:
        pickable = std::make_unique<Healer>(0.0f);
        break;
    case PickableType::LIGHTING_BOLT:
        pickable = std::make_unique<LightingBolt>(0.0f, 0.0f);
        break;
    case PickableType::CONFUSER:
        pickable = std::make_unique<Confuser>(0, 0.0f);
        break;
    case PickableType::FIREBALL:
        pickable = std::make_unique<FireBall>(0.0f, 0.0f);
        break;
    };
    pickable->load(t_zip);
    return std::move(pickable);
}

//================================================== pickable ==================================================//
//================================================== GUI ==================================================//

void Gui::save(TCODZip &t_zip)
{
    t_zip.putInt(m_log.size());
    for (auto it_msg{m_log.begin()}; it_msg < m_log.end(); it_msg++)
    {
        t_zip.putString((*it_msg)->m_text->c_str());
        t_zip.putColor(&(*it_msg)->m_color);
    }
}

void Gui::load(TCODZip &t_zip)
{
    int num_msg{t_zip.getInt()};
    while (num_msg > 0)
    {
        const char *text{t_zip.getString()};
        TCODColor color{t_zip.getColor()};
        message(color, text);
        num_msg--;
    }
}

//================================================== gui ==================================================//
//================================================== ENGINE ==================================================//
const int SAVEGAME_VERSION{0x1100};
void Engine::load(const bool t_pause)
{
    TCODZip zip;
    m_gui->m_menu.clear();
    // New Game option
    m_gui->m_menu.addItem(Menu::MenuItemCode::NEW_GAME, "New Game");

    if (TCODSystem::fileExists("game.sav"))
    {
        zip.loadFromFile("game.sav");
        int version{zip.getInt()};
        if (SAVEGAME_VERSION == version)
        {
            m_gui->m_menu.addItem(Menu::MenuItemCode::CONTINUE, "Continue");
        }
    }
    // Exit option
    m_gui->m_menu.addItem(Menu::MenuItemCode::EXIT, "Exit");

    // Ask for user selection
    Menu::MenuItemCode menu_item{m_gui->m_menu.pick(t_pause ? Menu::DisplayMode::PAUSE : Menu::DisplayMode::MAIN)};
    if (menu_item == Menu::MenuItemCode::EXIT || menu_item == Menu::MenuItemCode::NONE)
    {
        // Exit or window closed
        exit(0);
    }
    else if (menu_item == Menu::MenuItemCode::NEW_GAME)
    {
        term();
        init();
    }
    else
    {
        // Continue saved game
        term();

        // Load map
        m_level = zip.getInt();
        int w{zip.getInt()};
        int h{zip.getInt()};
        m_map = std::make_unique<Map>(w, h);
        m_map->load(zip);

        // Load player
        m_actors.emplace_back(Actor::s_builder.createActor(0, 0, 0, "", TCODColor::white).construct());
        m_player = m_actors.back().get();
        m_player->load(zip);

        // Load stairs
        m_actors.emplace_back(Actor::s_builder.createActor(0, 0, 0, "", TCODColor::white).construct());
        m_stairs = m_actors.back().get();
        m_stairs->load(zip);

        // Actors
        int num_actors{zip.getInt()};
        while (num_actors > 0)
        {
            // Create dummies
            m_actors.emplace_back(Actor::s_builder.createActor(0, 0, 0, "", TCODColor::white).construct());
            m_actors.back()->load(zip);
            num_actors--;
        }

        // Load message log
        m_gui->load(zip);

        // Force FOV recomputation
        m_game_status = Engine::GAME_STATUS::STARTUP;
    }
}

void Engine::save()
{
    // Handle permadeath
    if (m_player->m_destructible->isDead())
    {
        TCODSystem::deleteFile("game.sav");
    }
    else
    {
        TCODZip zip;
        zip.putInt(SAVEGAME_VERSION);
        zip.putInt(m_level);
        // Save map
        zip.putInt(m_map->m_width);
        zip.putInt(m_map->m_height);
        m_map->save(zip);

        // Save player
        m_player->save(zip);

        // Save stairs
        m_stairs->save(zip);

        // Save actors (-2 for the player and stairs)
        zip.putInt(m_actors.size() - 2);
        for (auto it_actor{m_actors.begin()}; it_actor < m_actors.end(); it_actor++)
        {
            Actor *ptr_actor{(*it_actor).get()};
            if (ptr_actor != m_player && ptr_actor != m_stairs)
            {
                (*it_actor)->save(zip);
            }
        }

        // Save message log
        m_gui->save(zip);
        zip.saveToFile("game.sav");
    }
}

//================================================== engine ==================================================//