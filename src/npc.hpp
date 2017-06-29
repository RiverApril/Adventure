#ifndef npc_hpp
#define npc_hpp

#include "entity.hpp"
#include "dialog.hpp"

class Npc : public Entity {
public:
    
    Npc(int x, int y, unsigned char color, char sym, const char* name, Dialog* dialog) : Entity(x, y), color(color), sym(sym), name(name), dialog(dialog){
        this->dialog->shouldBeDeleted = false;
        this->dialog->speaker = name;
    }
    virtual ~Npc(){
        delete dialog;
    }
    
    unsigned char color;
    char sym;
    const char* name;
    Dialog* dialog;
    
    void update(){
        Entity::update();
        putCharA(R(x), R(y), color, sym);
    }
    
};

#endif
