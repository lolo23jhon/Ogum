#ifndef utl_HPP
#define utl_HPP
#include <string>
#include <math.h>

namespace utl
{

// Checks of a word starts with a vowel
static bool startsVowel(std::string &t_word)
{
    const std::string vowels{"AEIOUaeiou"};

    return vowels.find(t_word[0]) != std::string::npos;
}

static bool isOne(const int t_x)
{
    return t_x == 1;
}

static float distance(const int t_x1, const int t_y1, const int t_x2, const int t_y2)
{
    return sqrtf(powf((float)(t_x2 - t_x1), 2.0f) + powf((float)(t_y2 - t_y1), 2.0f));
}

}; // namespace utl
#endif