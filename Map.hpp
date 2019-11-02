#ifndef MAP_GEN_HPP
#define MAP_GEN_HPP
#include <vector>
#include <memory>
#include "libtcod/bsp.hpp"
#include "libtcod/fov.hpp"

class Engine;
class Map;

static const int ROOM_MAX_SIZE{12};
static const int ROOM_MIN_SIZE{6};
static const int MAX_ROOM_MONSTERS{3};

class BspListener : public ITCODBspCallback
{
private:
    Map &m_map;
    int m_room_num;

    // Center of last room
    int m_last_x;
    int m_last_y;

public:
    BspListener(Map &t_map) : m_map{t_map}, m_room_num{0} {}
    bool visitNode(TCODBsp *t_node, void *t_userData);
};

struct Tile
{
    bool m_explored; // Has the player seen the tile before?

    // By default are new tiles are unexplored.
    Tile() : m_explored{false} {}
};

class Map
{

public:
    const int m_width;
    const int m_height;

    Map(const int t_width, const int t_HPPeight);
    bool isWall(const int t_x, const int t_y) const;
    bool canWalk(const int t_x, const int t_y) const;
    bool isExplored(const int t_x, const int t_y) const;
    bool isInFov(const int t_x, const int t_y) const;
    void computeFov() const;
    void init(const bool t_withActors);
    void save(TCODZip &t_zip);
    void load(TCODZip &t_zip);

protected:
    std::unique_ptr<TCODMap> m_tcod_map;
    mutable std::vector<Tile> m_tiles;
    long m_rng_seed;
    std::unique_ptr<TCODRandom> m_rng;

    void dig(int t_x1, int t_y1, int t_x2, int t_y2);
    void createRoom(const bool t_first, const int t_x1, const int t_y1, const int t_x2, const int t_y2, const bool t_withActors);
    void render() const;
    void addMonster(const int t_x, const int t_y);
    void addItem(const int t_x, const int t_y);

    friend class Engine;
    friend class Gui;
    friend class PlayerAi;
    friend class MonsterAi;
    friend class BspListener;
};

#endif