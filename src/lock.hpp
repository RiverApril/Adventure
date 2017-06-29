#ifndef lock_hpp
#define lock_hpp

#include "entity.hpp"

struct LockDraw{
    LockDraw(unsigned char color, char sym) : color(color), sym(sym), backColor(0), backSym(0){}
    LockDraw(unsigned char color, char sym, char backSym) : color(color), sym(sym), backColor(color), backSym(backSym){}
    LockDraw(unsigned char color, char sym, unsigned char backColor, char backSym) : color(color), sym(sym), backColor(backColor), backSym(backSym){}
    unsigned char color;
    char sym;
    unsigned char backColor;
    char backSym;
};

class Lock : public Entity {
public:
    
    Lock(int pos, bool keyable, bool& locked, LockDraw drawLocked, LockDraw drawUnlocked, unsigned char backDir = 0) : Entity(AREA_X(pos), AREA_Y(pos)), keyable(keyable), locked(locked), drawLocked(drawLocked), drawUnlocked(drawUnlocked), backDir(backDir){}
    virtual ~Lock(){}
    
    bool keyable;
    bool& locked;
    LockDraw drawLocked;
    LockDraw drawUnlocked;
    unsigned char backDir;
    
    void update(){
        Entity::update();
        
        LockDraw drawState = locked?drawLocked:drawUnlocked;
        putCharA(R(x), R(y), drawState.color, drawState.sym);
        if(drawState.backSym != 0){
            putCharA(R(x)+DIR_X(backDir), R(y)+DIR_Y(backDir), drawState.backColor, drawState.backSym);
        }
    }
    
};

#endif
