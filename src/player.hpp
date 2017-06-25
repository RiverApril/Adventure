#ifndef player_hpp
#define player_hpp

#include "entity.hpp"


#define SWING_TIMER_MAX 0.2
#define SWING_COOLDOWN_TIMER_MAX 0.1

#define INVULNERABLE_TIMER_MAX 1.0

#define USE_BOW_TIMER_MAX 0.3

#define OUT_OF_SOMETHING_FLASH_TIMER_MAX 1.0


class Player : public Entity, public Living{
public:
    
    float swingCooldownTimer = 0;
    float swingTimer = 0;
    float invulnerableTimer = 0;
    float useTimer = 0;
    float outOfSomethingFlashTimer = 0;
    
    unsigned char faceDir = 0;
    
    
    Player(int x, int y) : Entity(x, y), Living(6){}
    virtual ~Player(){}

    void update();
    bool receiveAttack(Entity* attacker, int damage, int type);

};

#endif
