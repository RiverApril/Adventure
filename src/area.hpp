#ifndef area_hpp
#define area_hpp

#include "global.hpp"

#define areaW consoleW
#define areaH consoleHH

#define DIR_E 0
#define DIR_N 1
#define DIR_W 2
#define DIR_S 3

#define PORTAL_POS_E (-1)
#define PORTAL_POS_N (-2)
#define PORTAL_POS_W (-3)
#define PORTAL_POS_S (-4)

#define DIR_X(d) ((d)==DIR_E?1:((d)==DIR_W?-1:0))
#define DIR_Y(d) ((d)==DIR_S?1:((d)==DIR_N?-1:0))

#define AREA_POS(x, y) ((y)*areaW+(x))
#define AREA_X(p) ((p) % areaW)
#define AREA_Y(p) ((p) / areaW)

#define TILE_TYPE_AIR 0
#define TILE_TYPE_SOLID 1

struct Tile{
    Tile(){}
    Tile(char symbol, unsigned char type, unsigned char color) : symbol(symbol), type(type), color(color){}
    char symbol;
    unsigned char type;
    unsigned char color;

    bool isSolid(){
        switch(type){
            case TILE_TYPE_AIR: return false;
            case TILE_TYPE_SOLID: return true;
            default: return false;
        }
    }
    
    bool receiveAttack(int damage, int type){
        return false;
    }
};

struct Area;
class Entity;

struct Portal{
    Portal(char id, int pos, Area* out) : id(id), pos(pos), out(out){}
    char id;
    int pos;
    Area* out;
};

struct Poi{
    Poi(char sym, int pos) : sym(sym), pos(pos){};
    char sym;
    int pos;
};

struct Area{

    int index;
    const char* name;
    Tile (&tiles)[areaW*areaH];
    int portalCount;
    Portal* portals;
    int poiCount;
    Poi* pois;
    void (*onUpdate)() = nullptr;
    void (*onEnter)(char) = nullptr;
    void (*onLeave)(char) = nullptr;
    std::vector<Entity*> entities;

    Area(int index, const char* name, Tile (&tiles)[areaW*areaH], int portalCount, Portal* portals
        , int poiCount, Poi* pois, void (*onUpdate)(), void (*onEnter)(char), void (*onLeave)(char)) : index(index), name(name)
    , tiles(tiles), portalCount(portalCount), portals(portals), poiCount(poiCount), pois(pois)
    , onUpdate(onUpdate), onEnter(onEnter), onLeave(onLeave){}


    void update();
    void enter(char sym);
    void leave(char sym);
    
    void drawTiles();
    
    bool isPlaceSolid(int x, int y, Entity* from, bool(*isEntitySolid)(Entity*));
    bool attackPlace(int x, int y, Entity* attacker, int damage, int type, bool(*isEntityTarget)(Entity*));
    
    Tile tileAt(int x, int y) {
        int i = y*areaW+x;
        if(i >= 0 && i < areaW*areaH){
            return (tiles[i]);
        }else{
            return Tile(' ', TILE_TYPE_AIR, C_WHITE);
        }
    }
};


#endif
