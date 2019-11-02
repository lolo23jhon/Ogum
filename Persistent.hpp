#ifndef PERSISTENT_HPP
#define PERSISTENT_HPP

class Persistent
{
public:
    virtual void load(TCODZip &t_zip) = 0;
    virtual void save(TCODZip &t_zip) = 0;
};

#endif