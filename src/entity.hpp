#ifndef entity_hpp
#define entity_hpp

#include "global.hpp"

#define R(x) ((int)round(x))

#define MARK_NONE 0
#define MARK_REMOVE_NOT_DELETE 1
#define MARK_DELETE 2

#define ATTACK_TYPE_NORMAL 0
#define ATTACK_TYPE_EXPLOSIVE 1
#define ATTACK_TYPE_STUN 2

class Entity{
public:

    Entity(int x, int y) : x(x), y(y){};
    virtual ~Entity(){}

    float x, y;
    float animTick = 0;
    unsigned char dir = 0;
    unsigned char mark = MARK_NONE;

    virtual void update(){
        animTick += delta * ANIM_TICK_LENGTH;
    }

};

class Living;

class CanBeHit{
public:
    CanBeHit(){}
    virtual ~CanBeHit(){}
    virtual bool receiveAttack(Entity* attacker, int damage, int type){return false;}
    
    bool shouldBeSolid();
};

class Living : public CanBeHit {
public:
    
    Living(int hp) : hp(hp), CanBeHit(){maxHp=hp;}
    virtual ~Living(){}
    
    int maxHp;
    int hp;
};

class Stunnable{
public:
    Stunnable(float maxStunTimer) : maxStunTimer(maxStunTimer){}
    virtual ~Stunnable(){}
    
    void stun(){
        stunned = true;
        stunTimer = maxStunTimer;
    }
    
    void update(){
        if(stunTimer <= 0){
            stunned = false;
        }else{
            stunTimer -= delta;
        }
    }
    
    const float maxStunTimer;
    float stunTimer = 0;
    bool stunned = false;
    
};

class Projectile{
public:
    Projectile(){}
    virtual ~Projectile(){}
};


struct ColorSym{
    ColorSym(unsigned char color, char sym) : color(color), sym(sym){}
    unsigned char color;
    char sym;
};


#endif
