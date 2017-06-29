#include "bomb.hpp"

#include "area.hpp"
#include "player.hpp"

bool isTargetForBomb(Entity* e){
    return dynamic_cast<CanBeHit*>(e) && !dynamic_cast<Player*>(e);
}

Bomb::Bomb(int x, int y) : Entity(x, y){
    fuseTimer = 5;
    boomTimer = 0.2;
}

void Bomb::update(){
    Entity::update();
    
    if(fuseTimer > 0){
        fuseTimer -= delta;
        if(fuseTimer < 1){
            putCharA(x, y, useColor(USE_BOMB), int(animTick) % 2 == 0 ? '*' : 'o');
        }else{
            putCharA(x, y, useColor(USE_BOMB), int(animTick) % 4 == 0 ? '*' : 'o');
        }
    }else{
        if(boomTimer > 0){
            boomTimer -= delta;
            char sm = boomTimer < 0.1 ?'*':'.';
            char lg = boomTimer < 0.1 ?'#':'*';
            int rx = round(x);
            int ry = round(y);
            for(int i=-1;i<=1;i++){
                for(int j=-1;j<=1;j++){
                    putCharA(rx+i, ry+j, useColor(USE_BOMB), abs(i)+abs(j)==2?sm:lg);
                }
            }
        }else{
            for(int i=-1;i<=1;i++){
                for(int j=-1;j<=1;j++){
                    activeArea->attackPlace(R(x), R(y), this, 4, ATTACK_TYPE_EXPLOSIVE, isTargetForBomb);
                }
            }
            mark = MARK_DELETE;
        }
    }

}
