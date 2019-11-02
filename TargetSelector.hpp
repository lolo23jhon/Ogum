#ifndef TARGET_SELECTOR_HPP
#define TARGET_SELECTOR_HPP

class Actor;

class TargetSelector
{
public:
    enum SelectorType
    {
        CLOSEST_MONSTER,
        SELECTED_MONSTER,
        USER_RANGE,
        SELECTED_RANGE
    };

    TargetSelector(const SelectorType t_type, const float t_range);
    void selectTargets(Actor *t_user, std::vector<Actor *> &t_out_targets);

protected:
    SelectorType m_type;
    float m_range;
};

#endif