#ifndef GUI_HPP
#define GUI_HPP
#include <memory>
#include <vector>
#include <stdarg.h>
#include "libtcod/color.hpp"

class Engine;
class TCODConsole;
class TCODColor;

class Menu
{
public:
    enum class MenuItemCode
    {
        NONE,
        NEW_GAME,
        CONTINUE,
        EXIT,
        CONSTITUTION,
        STRENGTH,
        AGILITY
    };

    enum class DisplayMode
    {
        MAIN,
        PAUSE
    };

    ~Menu();
    void clear();
    void addItem(const MenuItemCode t_code, const char *t_label);
    MenuItemCode pick(const DisplayMode t_mode = DisplayMode::MAIN);

protected:
    struct MenuItem
    {
        MenuItemCode m_code;
        std::string m_label;

        MenuItem(const MenuItemCode t_code, const char *t_label) : m_code(t_code), m_label{t_label} {}
    };
    std::vector<std::unique_ptr<MenuItem>> m_items;
};

class Gui : public Persistent
{
public:
    Menu m_menu;

    Gui();
    ~Gui();
    void render();
    void clear();
    void message(const TCODColor &t_color, const char *t_text, ...);
    void save(TCODZip &t_zip);
    void load(TCODZip &t_zip);

protected:
    struct Message
    {
        std::unique_ptr<std::string> m_text;
        const TCODColor m_color;

        Message(const char *t_text, const TCODColor &t_color);
    };

    std::vector<std::unique_ptr<Message>> m_log;
    std::unique_ptr<TCODConsole> m_con;

    void renderBar(const int t_x, const int t_y, const int t_width,
                   const char *t_name, const float t_value, const float t_max_value,
                   const TCODColor &t_bar_color, const TCODColor &t_back_color);
    void renderMouseLook();
};

#endif