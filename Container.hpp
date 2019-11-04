#ifndef CONTAINER_HPP
#define CONTAINER_HPP
#include <vector>
#include <memory>

class Actor;

class Container : public Persistent
{
public:
    int m_size; // Unlimited if 0
    std::vector<std::unique_ptr<Actor>> m_inventory;

    Container(const int t_size);
    virtual ~Container();
    bool hasSpace();
    bool add(std::unique_ptr<Actor> t_actor);
    void remove(Actor *t_owner, Actor *t_actor);
    std::unique_ptr<Actor> move(Actor *t_owner, Actor *t_actor);
    void save(TCODZip &t_zip);
    void load(TCODZip &t_zip);
};

#endif