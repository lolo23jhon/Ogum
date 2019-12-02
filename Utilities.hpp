#ifndef UTILITIES_HPP
#define UTILITIES_HPP

namespace utils
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

static bool fexists(const char* t_fileName) {
	std::ifstream ifile(t_fileName);
	return (bool)ifile;
}

template <size_t N, class T>
std::array<T, N> make_array(const T& v) {
	std::array<T, N> ret;
	ret.fill(v);
	return std::move(ret);
}


}; // namespace utils
#endif