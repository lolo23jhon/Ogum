#include "main.hpp"
#include <algorithm>

///////////////////////////////////////////////////// ENGINE::S_ENGINE /////////////////////////////////////////////////////////////////

Engine *Engine::s_engine{nullptr};
TCODRandom* s_rng{ TCODRandom::getInstance() };

///////////////////////////////////////////////////// engine::s_engine /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ENGINE::ENGINE /////////////////////////////////////////////////////////////////

Engine::Engine(const int t_screen_w, const int t_screen_h)
    : m_player{nullptr},
      m_gui{nullptr},
      m_screen_w{t_screen_w},
      m_screen_h{t_screen_h},
      m_fovRadius{10},
      m_level{1},
      m_game_status{GAME_STATUS::STARTUP}
{
    s_engine = this;
    TCODSystem::setFps(24);
    // Root console initialization
    TCODConsole::initRoot(m_screen_w, m_screen_h, "Ogum v0.1", false);
    m_gui = std::make_unique<Gui>();
}

///////////////////////////////////////////////////// engine::engine /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ENGINE::~ENGINE /////////////////////////////////////////////////////////////////

Engine::~Engine()
{
    term();
    m_gui.reset();
}

///////////////////////////////////////////////////// engine::~engine /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ENGINE::UPDATE /////////////////////////////////////////////////////////////////

void Engine::update()
{

    if (m_game_status == GAME_STATUS::STARTUP)
    {
        m_map->computeFov();
    }
    m_game_status = GAME_STATUS::IDLE;

    // Checking just keyboard input
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &m_last_key, &m_mouse);
    // Press ESC to return to main menu
    if (m_last_key.vk == TCODK_ESCAPE)
    {
        save();
        load(true);
    }
    m_player->update();

    // If this is a new turn, we iterate over all the actors (but the player) and call the update function.
    // This must occur after the fov computation because monsters may need to know whether they're in the
    // player field of view. Note that even if the player didn't actually move (he hit a wall or a creature),
    // there is a new turn.
    if (m_game_status == GAME_STATUS::NEW_TURN)
    {
        for (auto it_actor{m_actors.begin()}; it_actor < m_actors.end(); it_actor++)
        {
            if ((*it_actor).get() != m_player)
            {
                (*it_actor)->update();
            }
        }
    }
}

///////////////////////////////////////////////////// engine::update /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ENGINE::RENDER /////////////////////////////////////////////////////////////////

void Engine::render()
{
    // Clear the console
    TCODConsole::root->clear();

    // Draw the map
    m_map->render();

    // Draw the actors
    for (auto it_actor{m_actors.begin()}; it_actor < m_actors.end(); it_actor++)
    {
        if ((*it_actor).get() != m_player && ((!(*it_actor)->m_fov_only && m_map->isExplored((*it_actor)->m_x, (*it_actor)->m_y)) || m_map->isInFov((*it_actor)->m_x, (*it_actor)->m_y)))
        {
            (*it_actor)->render();
        }
    }

    // Always draw the player last
    m_player->render();
    // Show the gui
    m_gui->render();
}

///////////////////////////////////////////////////// engine::render /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ENGINE::SENDTOBACK /////////////////////////////////////////////////////////////////

void Engine::sendToBack(Actor *t_actor)
{
    for (auto it_actor{m_actors.begin()}; it_actor < m_actors.end(); it_actor++)
    {
        if ((*it_actor).get() == t_actor)
        {
            std::rotate(m_actors.begin(), it_actor, it_actor + 1);
        }
    }
}

///////////////////////////////////////////////////// engine::sendtoback /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ENGINE::GETCLOSESTMONSTER /////////////////////////////////////////////////////////////////

Actor *Engine::getClosestMonster(const int t_x, const int t_y, const float t_range) const
{
    Actor *closest{nullptr};
    float bestDistance{1E6f};

    for (auto it_actor{m_actors.begin()}; it_actor < m_actors.end(); it_actor++)
    {
        if ((*it_actor).get() != m_player && (*it_actor)->m_destructible && !(*it_actor)->m_destructible->isDead())
        {
            float distance{(*it_actor)->getDistance(t_x, t_y)};
            if (distance < bestDistance && (distance <= t_range || t_range == 0.0f))
            {
                bestDistance = distance;
                closest = (*it_actor).get();
            }
        }
    }
    return closest;
}

///////////////////////////////////////////////////// engine::getclosestmonster /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ENGINE::PICKATILE /////////////////////////////////////////////////////////////////

bool Engine::pickATile(int *t_x, int *t_y, const float t_max_range)
{
    // Make a separate loop because we need to keep renderin the screen
    while (!TCODConsole::isWindowClosed())
    {
        render();
        // Highligh possible range
        for (int x{0}; x < m_map->m_width; x++)
        {
            for (int y{0}; y < m_map->m_height; y++)
            {
                if (m_map->isInFov(x, y) && (t_max_range == 0 || m_player->getDistance(x, y) <= t_max_range))
                {
                    TCODColor color{TCODConsole::root->getCharBackground(x, y)};
                    color = color * 1.2f;
                    TCODConsole::root->setCharBackground(x, y, color);
                }
            }
        }

        TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &m_last_key, &m_mouse);

        if (m_map->isInFov(m_mouse.cx, m_mouse.cy) &&
            (t_max_range == 0 || m_player->getDistance(m_mouse.cx, m_mouse.cy) <= t_max_range))
        {
            TCODConsole::root->setCharBackground(m_mouse.cx, m_mouse.cy, TCODColor::white);
            if (m_mouse.lbutton_pressed)
            {
                *t_x = m_mouse.cx;
                *t_y = m_mouse.cy;
                return true;
            }
        }
        // Cancel if any key or right mouse is pressed
        if (m_mouse.rbutton_pressed /*|| m_last_key.vk != TCODK_NONE*/) // Should be TCODK_NONE, but it defaults to TCODK_TEXT for a reason
        {
            return false;
        }
        TCODConsole::flush();
    }
    return false;
}

///////////////////////////////////////////////////// engine::pickatile /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ENGINE::GETACTOR /////////////////////////////////////////////////////////////////

Actor *Engine::getActor(const int t_x, const int t_y) const
{
    for (auto it_actor{m_actors.begin()}; it_actor < m_actors.end(); it_actor++)
    {
        if ((*it_actor)->m_x == t_x && (*it_actor)->m_y == t_y && (*it_actor)->m_destructible && !(*it_actor)->m_destructible->isDead())
        {
            return (*it_actor).get();
        }
    }
    return nullptr;
}

///////////////////////////////////////////////////// engine::getactor /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ENGINE::INIT /////////////////////////////////////////////////////////////////

void Engine::init()
{

    // Make the player
    m_actors.emplace_back(Actor::s_builder.createActor(40, 25, '@', "player", TCODColor::white)
                              .setDesctrucible<PlayerDestructible>(30.0f, 2.0f, "your corpse", 0)
                              .setAttacker(5.0f)
                              .setAi<PlayerAi>()
                              .setContainer(26)
                              .build());
    m_player = m_actors.back().get();

    // Add stairs
    m_actors.emplace_back(Actor::s_builder.createActor(0, 0, '>', "stairs", TCODColor::white, true, false).build());
    m_stairs = m_actors.back().get();

    // The map goes after because it needs the m_player from its s_engine ptr
    m_map = std::make_unique<Map>(80, 43);
    m_map->init(true);
    m_gui = std::make_unique<Gui>();

    // Welcome message
    m_gui->message(TCODColor::red, "Welcome stranger!\nPrepare to perish in the Crypt of the Ancients.");
    m_game_status = GAME_STATUS::STARTUP;
}

///////////////////////////////////////////////////// engine::init /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ENGINE::TERM /////////////////////////////////////////////////////////////////

void Engine::term()
{
    // Reset all actor pointers
    m_actors.clear();

    // Reset map
    if (m_map)
    {
        m_map.reset();
    }

    // Reset the gui (clear console...)
    m_gui->clear();
}

///////////////////////////////////////////////////// engine::term /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ENGINE::NEXTLEVEL/////////////////////////////////////////////////////////////////

void Engine::nextLevel()
{
    m_level++;
    // Heal the player halph of max hp
    m_gui->message(TCODColor::lightViolet, "You take a moment to rest and recover you strength.");
    m_player->m_destructible->heal(m_player->m_destructible->m_max_hp / 2);
    m_gui->message(TCODColor::red, "After a rare moment of peace, you descend\ndeeper into the heart of the dunegon...");

    // Delete the map
    m_map.reset();

    // Move the player to a tmp value
    std::unique_ptr<Actor> tmp_player{std::move(
        *std::make_move_iterator(
            std::find_if(m_actors.begin(),
                         m_actors.end(),
                         [this](std::unique_ptr<Actor> &t_actor) { return t_actor.get() == m_player; })))};

    // Move the stairs to a tmp value
    std::unique_ptr<Actor> tmp_stairs{std::move(
        *std::make_move_iterator(
            std::find_if(m_actors.begin(),
                         m_actors.end(),
                         [this](std::unique_ptr<Actor> &t_actor) { return t_actor.get() == m_stairs; })))};

    // Clear all actors
    m_actors.clear();

    // Put the player back
    m_actors.emplace_back(std::move(tmp_player));
    m_player = m_actors.back().get();

    // Put the stairs back
    m_actors.emplace_back(std::move(tmp_stairs));
    m_stairs = m_actors.back().get();

    // Create a new map (with actors)
    m_map = std::make_unique<Map>(80, 43);
    m_map->init(true);
    m_game_status = GAME_STATUS::STARTUP;
}

///////////////////////////////////////////////////// engine::nextlevel/////////////////////////////////////////////////////////////////
