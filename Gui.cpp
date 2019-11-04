#include "main.hpp"


static const int PANEL_HEIGHT{7};
static const int BAR_WIDTH{20};
static const int MSG_X{BAR_WIDTH + 2};
static const int MSG_HEIGHT{PANEL_HEIGHT - 1};


Gui::Gui() : m_con{std::make_unique<TCODConsole>(Engine::s_engine->m_screen_w, PANEL_HEIGHT)} {}


Gui::~Gui()
{
    m_con.reset();
    clear();
}


void Gui::render()
{
    // Clear the gui console
    m_con->setDefaultBackground(TCODColor::black);
    m_con->clear();

    // Draw health bar
    renderBar(1, 1, BAR_WIDTH, "HP",
              Engine::s_engine->m_player->m_destructible->m_hp,
              Engine::s_engine->m_player->m_destructible->m_max_hp,
              TCODColor::lightRed,
              TCODColor::darkerRed);

    // Draw xp bar
    PlayerAi *p_ai{dynamic_cast<PlayerAi *>(Engine::s_engine->m_player->m_ai.get())};
    char xpTxt[128];
    sprintf(xpTxt, "XP(%d)", p_ai->m_xp_lvl);
    renderBar(1, 5, BAR_WIDTH, xpTxt, (float)Engine::s_engine->m_player->m_destructible->m_xp, (float)p_ai->getNextLevel(), TCODColor::lightViolet, TCODColor::darkViolet);

    // Draw the message log
    int y{1};
    float color_coeff{0.4f};
    for (auto it_msg{m_log.begin()}; it_msg < m_log.end(); it_msg++)
    {
        m_con->setDefaultForeground((*it_msg)->m_color * color_coeff);
        m_con->print(MSG_X, y, (*it_msg)->m_text->c_str());
        y++;
        if (color_coeff < 1.0f)
        {
            color_coeff += 0.3f;
        }
    }

    // Mouse look
    renderMouseLook();

    // Display dungeon level
    m_con->setDefaultForeground(TCODColor::white);
    m_con->print(3, 3, "Dungeon level %d", Engine::s_engine->m_level);

    // Blit the GUI console on the root console
    TCODConsole::blit(m_con.get(), 0, 0, Engine::s_engine->m_screen_w, PANEL_HEIGHT,
                      TCODConsole::root, 0, Engine::s_engine->m_screen_h - PANEL_HEIGHT);
}


void Gui::renderBar(const int t_x,
                    const int t_y,
                    const int t_width,
                    const char *t_name,
                    const float t_value,
                    const float t_max_value,
                    const TCODColor &t_bar_color,
                    const TCODColor &t_back_color)
{
    // Fill background
    m_con->setDefaultBackground(t_back_color);
    m_con->rect(t_x, t_y, t_width, 1, false, TCOD_BKGND_SET);

    int bar_w{(int)(t_value / t_max_value * t_width)};
    if (bar_w > 0)
    {
        // Draw the bar
        m_con->setDefaultBackground(t_bar_color);
        m_con->rect(t_x, t_y, bar_w, 1, false, TCOD_BKGND_SET);
    }

    // Print the text on top of the bar
    m_con->setDefaultForeground(TCODColor::white);
    m_con->printEx(t_x + t_width / 2, t_y, TCOD_BKGND_NONE, TCOD_CENTER,
                   "%s : %g/%g", t_name, t_value, t_max_value);
}


void Gui::message(const TCODColor &t_color, const char *t_text, ...)
{
    // Build the text
    va_list ap;
    char buffer[128];
    va_start(ap, t_text);
    vsprintf(buffer, t_text, ap);
    va_end(ap);

    char *line_begin{buffer};
    char *line_end;

    do
    {
        // Make room for the new message
        if (m_log.size() == MSG_HEIGHT)
        {
            m_log.erase(m_log.begin());
        }

        // Detect end of line
        line_end = strchr(line_begin, '\n');

        if (line_end)
        {
            *line_end = '\0';
        }

        // Add new message to the log
        m_log.emplace_back(std::make_unique<Message>(line_begin, t_color));

        // Go to next line
        line_begin = line_end + 1;

    } while (line_end);
}


void Gui::renderMouseLook()
{
    if (!Engine::s_engine->m_map->isInFov(Engine::s_engine->m_mouse.cx, Engine::s_engine->m_mouse.cy))
    {
        // If mouse is out of fov there's nothing to render
        return;
    }

    char buffer[128]{""};

    bool first{true};
    for (auto it_actor{Engine::s_engine->m_actors.begin()}; it_actor < Engine::s_engine->m_actors.end(); it_actor++)
    {
        if ((*it_actor)->m_x == Engine::s_engine->m_mouse.cx &&
            (*it_actor)->m_y == Engine::s_engine->m_mouse.cy)

        {
            if (!first)
            {
                strcat(buffer, ", ");
            }
            else
            {
                first = false;
            }
            strcat(buffer, (*it_actor)->m_name.c_str());
        }
    }
    // Display the list of actors under the mouse cursor
    m_con->setDefaultForeground(TCODColor::lightGrey);
    m_con->print(1, 0, buffer);
}


void Gui::clear()
{
    // Clear the log (dont's worry child, smart pointers got our backs)
    m_log.clear();
}


Gui::Message::Message(const char *t_text, const TCODColor &t_color)
    : m_text{std::make_unique<std::string>(t_text)}, m_color{t_color} {}


Menu::~Menu()
{
    clear();
}


void Menu::clear()
{
    // Clear each item
    m_items.clear();
}


void Menu::addItem(const MenuItemCode t_code, const char *t_label)
{
    m_items.emplace_back(std::make_unique<MenuItem>(t_code, t_label));
}


const int PAUSE_MENU_WIDTH{30};
const int PAUSE_MENU_HEIGHT{15};

Menu::MenuItemCode Menu::pick(const Menu::DisplayMode t_mode)
{
    int selected_item{0};
    int menu_x{0};
    int menu_y{0};

    if (t_mode == Menu::DisplayMode::PAUSE)
    {
        menu_x = Engine::s_engine->m_screen_w / 2 - PAUSE_MENU_WIDTH / 2;
        menu_y = Engine::s_engine->m_screen_h / 2 - PAUSE_MENU_HEIGHT / 2;
        TCODConsole::root->setDefaultForeground(TCODColor(200, 180, 50));
        TCODConsole::root->printFrame(menu_x, menu_y, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT, true, TCOD_BKGND_ALPHA(70), "menu");
        menu_x += 2;
        menu_y += 3;
    }
    else
    {
        static TCODImage img{"menu_background1.png"};
        img.blit2x(TCODConsole::root, 0, 0);
        menu_x = 10;
        menu_y = TCODConsole::root->getHeight() / 3;
    }

    while (!TCODConsole::isWindowClosed())
    {
        int current_item{0};
        for (auto it_menu_entry{m_items.begin()}; it_menu_entry < m_items.end(); it_menu_entry++)
        {
            if (current_item == selected_item)
            {
                // Set selected item to ligth orange
                TCODConsole::root->setDefaultForeground(TCODColor::lightOrange);
            }
            else
            {
                // Set unselected items to light grey
                TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
            }
            TCODConsole::root->print(menu_x, menu_y + current_item * 3, (*it_menu_entry)->m_label.c_str());
            current_item++;
        }
        TCODConsole::flush();

        // Check for keypresses
        TCOD_key_t key;
        // Check for input (not blocking because we need to be able to close the window)
        TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
        switch (key.vk)
        {
        case TCODK_UP:
            selected_item--;
            if (selected_item < 0)
            {
                selected_item = m_items.size() - 1;
            }
            break;
        case TCODK_DOWN:
            selected_item = (selected_item + 1) % m_items.size();
            break;
        case TCODK_ENTER:
            return m_items[selected_item]->m_code;
            break;
        default:
            break;
        };
    }
    return MenuItemCode::NONE;
}

