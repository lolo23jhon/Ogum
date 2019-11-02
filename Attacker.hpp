#ifndef ATTACKER_HPP
#define ATTACKER_HPP
#include <map>

class Actor;

enum class DamageType
{
    PHYSICAL,
    HEAT,
    COLD,
    ELECTRIC,
    TRUE
};

enum class AttackType
{
    BASIC,   // Most common attack
    STRONG,  // Stronger attack with a lesser chance
    SPECIAL, // Attacks with cooldown or usage restrition (like poisonous attack or attacks with effects)
    MAX_ATTACK_TYPES
};

class Attacker
{

protected:
    struct Attack
    {
        DamageType m_damage_type;
        TCOD_dice_t m_dice;
        std::string m_msg_verb; // "The skeleton punched, stabbed, cut... the player for x damage."
    };

public:
    std::unordered_map<AttackType, Attack> m_attack_table; // Contains all the attacks the entity can deal
    Attacker(const float t_power);

    void attack(Actor *t_owner, Actor *t_target);
    void save(TCODZip &t_zip);
    void load(TCODZip &t_zip);
};

#endif