#ifndef slime_hpp
#define slime_hpp

#include "entity.hpp"

class Slime : public Entity, public Living, public Stunnable{
public:
    
    float changeDirTimer = 0;
    float deathTimer = 1.0;
    bool animBig = false;
    
    Slime(int x, int y);
    virtual ~Slime(){}
    
    void update();
    bool receiveAttack(Entity* attacker, int damage, int type);
    
};

#endif
