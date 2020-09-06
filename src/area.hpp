#ifndef area_hpp
#define area_hpp

#include "global.hpp"
#include "player.hpp"

#define TILE_TYPE_AIR 0
#define TILE_TYPE_SOLID 1
#define TILE_TYPE_ABOVE 2

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
            case TILE_TYPE_ABOVE: return false;
            default: return false;
        }
    }
    
    bool isAbove(){
        return type == TILE_TYPE_ABOVE;
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

#define FADE_NONE 0
#define FADE_DISSOLVE 1
#define FADE_ZOOM 2
#define FADE_WIPE_S 3
#define FADE_WIPE_N 4
#define FADE_WIPE_E 5
#define FADE_WIPE_W 6

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
    
    Tile tileAt(int x, int y, Entity* from = nullptr) {
        if(x >= 0 && x < areaW && y >= 0 && y < areaH){
            return (tiles[y*areaW+x]);
        }else if(dynamic_cast<Player*>(from)){
            return Tile(' ', TILE_TYPE_AIR, C_WHITE);
        }else{
            return Tile(' ', TILE_TYPE_SOLID, C_WHITE);
        }
    }
};


#endif
