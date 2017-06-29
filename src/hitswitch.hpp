#ifndef hitswitch_hpp
#define hitswitch_hpp

#include "entity.hpp"

#define SWITCH_TYPE_TOGGLE 0
#define SWITCH_TYPE_STAY 1
#define SWITCH_TYPE_TIMED 2

#define SWITCH_IGNORE_TIME 0.25

class HitSwitch : public Entity, public CanBeHit{
public:
    
    HitSwitch(int pos, bool& active, unsigned char type, ColorSym drawOff, ColorSym drawOn) : Entity(AREA_X(pos), AREA_Y(pos)), active(active), type(type), drawOff(drawOff), drawOn(drawOn){}
    
    HitSwitch(int pos, bool& active, unsigned char type, float maxTime, ColorSym drawOff, ColorSym drawOn) : HitSwitch(pos, active, type, drawOff, drawOn){
        this->maxTime = maxTime;
    }
    
    virtual ~HitSwitch(){}
    
    bool& active;
    unsigned char type;
    ColorSym drawOff;
    ColorSym drawOn;
    float timer = 0;
    float maxTime = 3;
    
    void update(){
        Entity::update();
        
        ColorSym drawState = active?drawOn:drawOff;
        putCharA(R(x), R(y), drawState.color, drawState.sym);
        
        
        if(timer > 0){
            timer -= delta;
        }else{
            if(type == SWITCH_TYPE_TIMED){
                active = false;
            }
        }
    }
    
    bool receiveAttack(Entity* attacker, int damage, int type){
        hit();
        return true;
    }
    
    void hit(){
        if(timer <= 0){
            switch (type) {
                case SWITCH_TYPE_TOGGLE:
                    active = !active;
                    timer = SWITCH_IGNORE_TIME;
                    break;
                    
                case SWITCH_TYPE_STAY:
                    active = true;
                    break;
                    
                case SWITCH_TYPE_TIMED:
                    active = true;
                    timer = maxTime;
                    break;
            }
        }
    }
    
};

class Indicator : public Entity {
public:
    
    Indicator(int x, int y, bool& active, ColorSym drawOff, ColorSym drawOn) : Entity(x, y), active(active), drawOff(drawOff), drawOn(drawOn){}
    Indicator(int pos, bool& active, ColorSym drawOff, ColorSym drawOn) : Entity(AREA_X(pos), AREA_Y(pos)), active(active), drawOff(drawOff), drawOn(drawOn){}
    virtual ~Indicator(){}
    
    bool& active;
    ColorSym drawOff;
    ColorSym drawOn;
    
    void update(){
        Entity::update();
        
        ColorSym drawState = active?drawOn:drawOff;
        putCharA(R(x), R(y), drawState.color, drawState.sym);
    }
    
};

#endif
