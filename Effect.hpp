#ifndef EFFECT_HPP
#define EFFECT_HPP

class Actor;

class Effect
{
public:
    virtual bool applyTo(Actor *t_actor) = 0;
};

class HealthEffect : public Effect
{
public:
    float m_amount;
    std::string m_message;
    TCODColor m_msg_color;

    HealthEffect(const float t_amount, const char *t_message = NULL, const TCODColor &t_msg_color = TCODColor::lightGrey);
    bool applyTo(Actor *t_actor);
};

#endif