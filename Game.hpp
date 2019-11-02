#include <iostream>
#include <string>
#include <map>

namespace Game
{

const std::map<const int, const std::string> mainMenuOptions{
    {1, "New Game"},
    {2, "Load Game"},
    {3, "Create New Character"},
    {4, "Options"},
    {5, "Quit Game"}};

bool isPlaying{true};

void mainMenu()
{
    std::cout << "MAIN MENU" << '\n';
    auto it{mainMenuOptions.cbegin()};
    while (it != mainMenuOptions.cend())
    {
        std::cout << it->first << '\n';
        it++;
    }

    std::string choice{""};
    std::cin >> choice;
    std::cout << '\n'
              << "Hello" << '\n';
}

}; // namespace Game
