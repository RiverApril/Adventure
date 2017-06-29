#include "boomerang.hpp"

#include "area.hpp"
#include "player.hpp"

#define BOOMERANG_SPEED 15
#define BOOMERANG_DISTANCE 6

bool isSolidForBoomerang(Entity* e){
    CanBeHit* c = dynamic_cast<CanBeHit*>(e);
    return (c && c->shouldBeSolid()) && !dynamic_cast<Player*>(e);
}

bool isTargetForBoomerang(Entity* e){
    return isSolidForBoomerang(e);
}

Boomerang::Boomerang(int x, int y, unsigned char dir) : Entity(x, y), Projectile(){
    this->dir = dir;
    startX = x;
    startY = y;
}

void Boomerang::update(){
    Entity::update();

    const float spd = delta * BOOMERANG_SPEED;
    
    float dx;
    float dy;
    
    dx = (startX - x);
    dy = (startY - y);
    if((dx*dx+dy*dy) >= BOOMERANG_DISTANCE*BOOMERANG_DISTANCE){
        returnToPlayer = true;
    }
    
    if(returnToPlayer){
        dx = (activePlayer->x - x);
        dy = (activePlayer->y - y);
        float dis = sqrtf(dx*dx+dy*dy);
        dx *= spd/dis;
        dy *= spd/dis;
    }else{
        dx = DIR_X(dir) * spd;
        dy = DIR_Y(dir) * spd;
    }
    
    
    if(returnToPlayer){
        x += dx;
        y += dy;
    }else{
        bool moved = false;
        if(!activeArea->isPlaceSolid(R(x+dx), R(y), this, isSolidForBoomerang)){
            x += dx;
            moved |= dx!=0;
        }
        if(!activeArea->isPlaceSolid(R(x), R(y+dy), this, isSolidForBoomerang)){
            y += dy;
            moved |= dy!=0;
        }
        if(!moved){
            returnToPlayer = true;
        }
    }
    
    if(R(x) < 0 || R(y) < 0 || R(x) >= areaW || R(y) >= areaH){
        returnToPlayer = true;
        return;
    }
    
    if(R(x) == R(activePlayer->x) && R(y) == R(activePlayer->y)){
        boomerangOut = false;
        mark = MARK_DELETE;
        return;
    }
    
    if(activeArea->attackPlace(R(x+dx), R(y+dy), this, 0, ATTACK_TYPE_STUN, isTargetForBoomerang)){
        returnToPlayer = true;
    }
    
    putCharA(R(x), R(y), useColor(USE_BOOMERANG), (int(animTick)%2 == 0)?'L':'7');

}
