#ifndef META_HPP
#define META_HPP
#include <unordered_map>
#include <vector>
#include <string>
#include "libtcod.hpp"
#include "Engine.hpp"

namespace meta
{

// ===================================== MOVEMENT ===================================
// Note: Coords start at the upper left corner of the screen: y axis is inverted.
static const std::unordered_map<char, std::pair<int, int>> movement_directions{
    {'k', {0, -1}},  // N
    {'y', {-1, -1}}, // NW
    {'u', {1, -1}},  // NE
    {'j', {0, 1}},   // S
    {'b', {-1, 1}},  // SW
    {'n', {1, 1}},   // SE
    {'h', {-1, 0}},  // W
    {'l', {1, 0}}    // E
};

static const std::unordered_map<TCOD_keycode_t, std::pair<int, int>> numlock_movement_directions{
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

// ===================================== movement ===================================

// ===================================== STATISTICS ===================================
enum class CHARACTER_STAT // These stats are applied to the Character class instance.
{
    INT, // intelligence: basic scaling for mana and spells
    STR, // strength
    DEX, // dexterity
    PER, // perception
    MAX_CHARACTER_STATS
};

// ===================================== statistics ===================================
// ####################################################################################
// ===================================== QUALITY ======================================

enum class QLTY
{
    SCRAPPY,
    BROKEN,
    BLUNT,
    MUNDANE,
    DECENT,
    GOOD,
    EXCELLENT,
    MARVELOUS,
    PARAGON,
    CENTAURIC,
    MAX_QUALITIES
};

struct QualityModifiers
{
    const QLTY m_quality;
    const float m_value_factor;
    const signed int m_functional_bonus;
    const std::string m_tag; // ITS THIS GUY, DON'T USE STRING REFERENCE IN UNORDERED_MAP
};

static const std::unordered_map<QLTY, QualityModifiers> QLTY_MODS{
    {QLTY::SCRAPPY, {QLTY::SCRAPPY, 0.2f, -3, "scrappy"}},
    {QLTY::BROKEN, {QLTY::BROKEN, 0.4f, -2, "broken"}},
    {QLTY::BLUNT, {QLTY::BLUNT, 0.7f, -1, "blunt"}},
    {QLTY::MUNDANE, {QLTY::MUNDANE, 1.0f, 0, "mundane"}},
    {QLTY::DECENT, {QLTY::DECENT, 1.1f, +1, "decent"}},
    {QLTY::GOOD, {QLTY::GOOD, 1.3f, +2, "good"}},
    {QLTY::EXCELLENT, {QLTY::EXCELLENT, 1.3f, +2, "excellent"}},
    {QLTY::MARVELOUS, {QLTY::MARVELOUS, 1.5f, +3, "marvelous"}},
    {QLTY::PARAGON, {QLTY::PARAGON, 2.0f, +4, "paragon"}},
    {QLTY::CENTAURIC, {QLTY::CENTAURIC, 3.0f, +5, "centauric"}}};

// ===================================== quality ======================================
// ####################################################################################
// ===================================== MATERIALS ====================================
enum class MTRL
{
    SILVER,
    GOLD,
    PAPER,
    WOOD,
    BONE,
    COPPER,
    BRONZE,
    IRON,
    STEEL,
    ORICHALCHUM,
    MITHRIL,
    ADAMANTIUM,
    MAX_MATERIALS
};

struct MaterialModifiers
{
    const MTRL m_material;
    const float m_value_factor;
    const signed int m_functional_bonus;
    const std::string m_tag;
};

static const std::unordered_map<MTRL, MaterialModifiers> MTRL_MODS{
    {MTRL::SILVER, {MTRL::SILVER, 3.0f, +3, "silver"}},
    {MTRL::GOLD, {MTRL::GOLD, 10.0f, -4, "gold"}},
    {MTRL::PAPER, {MTRL::PAPER, 0.1f, -6, "paper"}},
    {MTRL::WOOD, {MTRL::WOOD, 0.3f, -3, "wood"}},
    {MTRL::COPPER, {MTRL::COPPER, 0.7f, +1, "copper"}},
    {MTRL::BRONZE, {MTRL::BRONZE, 0.9f, +2, "bronze"}},
    {MTRL::IRON, {MTRL::IRON, 1.0f, +3, "iron"}},
    {MTRL::STEEL, {MTRL::STEEL, 1.2f, +4, "steel"}},
    {MTRL::ORICHALCHUM, {MTRL::ORICHALCHUM, 1.5f, +5, "orichalchum"}},
    {MTRL::MITHRIL, {MTRL::MITHRIL, 3.0f, +5, "mithril"}},
    {MTRL::ADAMANTIUM, {MTRL::ADAMANTIUM, 20.0f, +6, "adamantium"}}};
// ===================================== materials ====================================
// ####################################################################################
// ===================================== TRAITS =======================================
// Can apply bonus, malus or trade-offs of mods.
enum class TRAIT
{
    // HP BONUS
    GOOD_SHAPE, // HP+

    // DEF BONUS
    TOUGH_SKIN, // DEF+

    // DEX BONUS
    FAST_HPPANDS, // DEX++
    KEEN_EYE,   // DEX+
    CLUMSY,     // DEX-

    // MIXED BONUS
    NIMBLE,      // DEX+, SPD-
    ANALYTICAL,  // INT+, SPD-
    SCALES,      // DEF++, HP+
    WEAK_VISION, // DEX-
    HEAVY_METAL, // HP++, DEF+++, DEX--, SPD---

    MAX_TRAITS,
};

struct TraitModifiers
{
};
// ===================================== traits =======================================
// ####################################################################################
// ===================================== RACE =========================================

enum class RACE
{
    ARGONIAN,
    AUTOMATON,
    BIRD,
    DRAKE,
    ELF,
    ENT,
    FUNGAL,
    HOBBIT,
    KAJITH,
    NAGA,
    ORC,
    SKELETON,
    TRIFFID,
    UNDEAD,
    WEREWOLF,
    WRAITH,
    MAX_RACES
};

struct RaceModifiers // NOT FINISHED
{
    const RACE m_race;
    const std::string m_tag;
    const std::vector<TRAIT> m_race_traits;
};

static const std::unordered_map<RACE, RaceModifiers> RACE_MODS{};
// ==================================== RACE =========================================
// ###################################################################################
}; // namespace meta

#endif