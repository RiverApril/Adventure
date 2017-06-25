#ifndef boomerang_hpp
#define boomerang_hpp

#include "entity.hpp"

class Boomerang : public Entity, public Projectile{
public:
    
    int startX, startY;
    bool returnToPlayer = false;
    
    Boomerang(int x, int y, unsigned char dir);
    virtual ~Boomerang(){}
    
    void update();
    
};

#endif
