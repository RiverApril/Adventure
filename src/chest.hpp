#ifndef chest_hpp
#define chest_hpp

#include "entity.hpp"

#define TREASURE_NONE 0
#define TREASURE_KEY 1
#define TREASURE_SWORD 2
#define TREASURE_USE 3

struct Treasure{
    Treasure(unsigned char type, int meta = 1) : type(type), meta(meta){}
    unsigned char type;
    int meta = 1;
    void activate();
};

class Chest : public Entity {
public:
    
    Chest(int pos, bool &opened, Treasure treasure, ColorSym drawClosed, ColorSym drawOpened) : Entity(AREA_X(pos), AREA_Y(pos)), opened(opened), treasure(treasure), drawClosed(drawClosed), drawOpened(drawOpened){}
    
    virtual ~Chest(){}
    
    bool& opened;
    Treasure treasure;
    ColorSym drawClosed;
    ColorSym drawOpened;
    char sym;
    
    void update(){
        Entity::update();
        
        ColorSym drawState = opened?drawOpened:drawClosed;
        putCharA(R(x), R(y), drawState.color, drawState.sym);
    }
    
    void open();
    
};


#endif
