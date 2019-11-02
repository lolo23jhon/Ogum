#include "main.hpp"
#include <windows.h>


#pragma comment(lib, "user32.lib")
int main()
{
    try
    {
        // Close the black console window
#if CONSOLE_WINDOW == 0
        HWND con_window{GetConsoleWindow()};
        ShowWindow(con_window, SW_HIDE);
#endif

        Engine engine(80, 50);

        // Try to load a previously saved game
        engine.load();

        // Testing gameloop
        while (!TCODConsole::isWindowClosed())
        {

            engine.update();
            engine.render();
            TCODConsole::flush();
        }
        // Save the game when the window is closed
        engine.save();
        return 0;
    }
    catch (const std::exception &exc)
    {
        std::cerr << "CAUGHT EXCEPTION IN MAIN: \"" << exc.what() << "\"\n";
        std::cin;
        return 1;
    }
    catch (...)
    {
        std::cerr << "CAUGHT UNKNOWN EXCEPTION IN MAIN";
        std::cin;
        return 1;
    }
}
/*   
TODO:
-Create a testing ground
-Implement simple movement
-Implement character 

NOTES:
- DON'T use std::string & as struct members in unordered_map, it breaks the compiler

- When having circular dependencies, remember to forward declare.

- For adding items from std::initializer_ list taken as argument to a member container use a std::move_iterator and 
    std::vector.emplace_back().

- Another and more refined method is using std::vector.insert with move iterators. 

- When taking ownwership of class that has smart pointers as elements, pass them as refs and use std::move
    inside make_unique/make_shared, else it will try to copy with deleted copy constructor.

- DLLs go in the bin folder

- Static libs go as input files in linker flags in the batch file

- Globals inside headers should be declared as static (const automatically also makes them static)
    else you'll linker errors due to redefinition every time their headers are included.

- When having problems with a constructor ALWAYS check first if its public, every dragonthongue error is a 
    red herring.

- TCODConsole type does not seem to like STL container allocation, using new and delete for now

- printf seems to be faster that std::cout, even with std::string.c_str()

- ~ is typed with alt gr +

- https://www.internalpointers.com/post/move-smart-pointers-and-out-functions-modern-c: 
    "Pass [smart pointers] by reference when the function is supposed to modify the ownership of existing smart pointers"
*/
