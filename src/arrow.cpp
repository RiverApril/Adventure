#include "arrow.hpp"

#include "area.hpp"
#include "player.hpp"

#define ARROW_SPEED 20

bool isSolidForArrow(Entity* e){
    Living* l = dynamic_cast<Living*>(e);
    return l && l->hp > 0 && !dynamic_cast<Player*>(e);
}

bool isTargetForArrow(Entity* e){
    return dynamic_cast<Living*>(e) && !dynamic_cast<Player*>(e);
}

Arrow::Arrow(int x, int y, unsigned char dir) : Entity(x, y), Projectile(){
    this->dir = dir;
}

void Arrow::update(){
    Entity::update();
    
    const float spd = delta * ARROW_SPEED;
    
    float dx = DIR_X(dir) * spd;
    float dy = DIR_Y(dir) * spd;
    
    if(!activeArea->isPlaceSolid(R(x+dx), R(y+dy), this, isSolidForArrow)){
        x += dx;
        y += dy;
    }else{
        mark = MARK_DELETE;
        return;
    }
    if(x < 0 || y < 0 || x >= areaW || y >= areaH){
        mark = MARK_DELETE;
        return;
    }
    
    if(activeArea->attackPlace(R(x+dx), R(y+dy), this, 1, ATTACK_TYPE_NORMAL, isTargetForArrow)){
        mark = MARK_DELETE;
        return;
    }
    putCharA(R(x), R(y), useColor(USE_BOW), DIR_X(dir)==0?'|':'-');
}
