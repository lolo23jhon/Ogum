#ifndef PICKABLE_HPP
#define PICKABLE_HPP

class Pickable : public Persistent
{

public:
    bool pick(Actor *t_owner, Actor *t_carrier);
    virtual bool use(Actor *t_owner, Actor *t_carrier);
    void drop(Actor *t_owner, Actor *t_dropper);
    static std::unique_ptr<Pickable> create(TCODZip &t_zip);

protected:
    enum PickableType
    {
        HEALER,
        LIGHTING_BOLT,
        FIREBALL,
        CONFUSER
    };
};

class Healer : public Pickable
{
public:
    float m_amount; // Amount of health restored

    Healer(const float t_amount);
    bool use(Actor *t_owner, Actor *t_carrier);
    void save(TCODZip &t_zip);
    void load(TCODZip &t_zip);
};

class LightingBolt : public Pickable
{
public:
    float m_range;
    float m_damage;
    LightingBolt(const float t_range, const float t_damage);
    bool use(Actor *t_owner, Actor *t_user);
    void save(TCODZip &t_zip);
    void load(TCODZip &t_zip);
};

class FireBall : public LightingBolt
{
public:
    FireBall(const float t_range, const float t_damage);
    bool use(Actor *t_owner, Actor *t_user);
    void save(TCODZip &t_zip);
};

class Confuser : public Pickable
{
public:
    int m_num_turns;
    float m_range;

    Confuser(const int t_num_turns, const float t_range);
    bool use(Actor *t_owner, Actor *t_user);
    void save(TCODZip &t_zip);
    void load(TCODZip &t_zip);
};
#endif
