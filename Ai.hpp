#ifndef AI_HPP
#define AI_HPP
#include <unordered_map>

class Actor;

class Ai : public Persistent
{
public:
    virtual void update(Actor *t_owner) = 0;
    static std::unique_ptr<Ai> create(TCODZip &t_zip);

protected:
    enum AiType
    {
        MONSTER,
        CONFUSED_MONSTER,
        PLAYER
    };
};

class PlayerAi : public Ai
{
public:
    int m_xp_lvl;

    PlayerAi();
    int getNextLevel();
    void wait(const unsigned int t_num_turns = 1);
    void update(Actor *t_owner);
    void handleActionKey(Actor *t_owner, const int t_ascii, const bool t_shift = false);
    Actor *chooseFromInventory(Actor *t_owner);
    void load(TCODZip &t_zip);
    void save(TCODZip &t_zip);

protected:
    bool moveOrAttack(Actor *t_owner, const int t_target_x, const int t_target_y);
    static const std::unordered_map<TCOD_keycode_t, std::pair<int, int>> s_movement_keys;
    static const std::unordered_map<char, std::pair<int, int>> s_movement_chars;
};

class MonsterAi : public Ai
{
public:
    MonsterAi();
    void update(Actor *t_owner);
    void load(TCODZip &t_zip);
    void save(TCODZip &t_zip);

protected:
    int m_move_count;
    static const int S_TRACKING_TURNS;
    void moveOrAttack(Actor *t_owner, const int t_target_x, const int t_target_y);
};

class ConfusedMonsterAi : public Ai
{
public:
    ConfusedMonsterAi(const int t_num_turns, std::unique_ptr<Ai> t_old_ai);
    void update(Actor *t_owner);
    void load(TCODZip &t_zip);
    void save(TCODZip &t_zip);

protected:
    int m_num_turns;
    std::unique_ptr<Ai> m_old_ai;
};

#endif