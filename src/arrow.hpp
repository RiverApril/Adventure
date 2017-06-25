#ifndef arrow_hpp
#define arrow_hpp

#include "entity.hpp"

class Arrow : public Entity, public Projectile{
public:
    
    Arrow(int x, int y, unsigned char dir);
    virtual ~Arrow(){}
    
    void update();
    
};

#endif
