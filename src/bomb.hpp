#ifndef bomb_hpp
#define bomb_hpp

#include "entity.hpp"

class Bomb : public Entity {
public:
    
    float fuseTimer = 0;
    float boomTimer = 0;
    
    Bomb(int x, int y);
    virtual ~Bomb(){}
    
    void update();
    
};

#endif
