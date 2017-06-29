#include "slime.hpp"
#include "global.hpp"

#include "area.hpp"
#include "player.hpp"

#define SLIME_SPEED 1

bool isTargetForSlime(Entity* e){
    return dynamic_cast<Player*>(e);
}

bool isSolidForSlime(Entity* e){
    CanBeHit* c = dynamic_cast<CanBeHit*>(e);
    return (c && c->shouldBeSolid()) && !isTargetForSlime(e);
}

Slime::Slime(int x, int y) : Entity(x, y), Living(1), Stunnable(5){
    animTick = randf()*4;
    changeDirTimer = randf()*2;
    dir = rand()%4;
}

void Slime::update(){
    Entity::update();
    Stunnable::update();
    
    if(hp <= 0){
        if(deathTimer <= 0){
            mark = MARK_DELETE;
        }else{
            deathTimer -= delta;
            if(deathTimer >= 0.25){
                putCharA(R(x), R(y), C_WHITE, (int(animTick)%2 == 0)?'O':'o');
            }else{
                putCharA(R(x), R(y), C_WHITE, '*');
            }
        }
    }else if(stunned){
        
        if(stunTimer >= 1){
            putCharA(R(x), R(y), C_WHITE, animBig?'O':'o');
        }else{
            putCharA(R(x), R(y), C_WHITE, (int(animTick)%2 == 0)?'O':'o');
        }
        
    }else{
    
        const float spd = delta * SLIME_SPEED;
        
        if(changeDirTimer <= 0){
            do{
                dir = rand()%4;
            }while(activeArea->isPlaceSolid(R(x)+DIR_X(dir), R(y)+DIR_Y(dir), this, isSolidForSlime));
            changeDirTimer = 1+randf()*2;
        }else{
            changeDirTimer -= delta;
        }
        
        float dx = DIR_X(dir) * spd;
        float dy = DIR_Y(dir) * spd;
        
        if(!activeArea->isPlaceSolid(R(x+dx), R(y), this, isSolidForSlime)) {
            x += dx;
        }
        if(!activeArea->isPlaceSolid(R(x), R(y+dy), this, isSolidForSlime)) {
            y += dy;
        }
        
        activeArea->attackPlace(R(x), R(y), this, 1, ATTACK_TYPE_NORMAL, isTargetForSlime);
        
        putCharA(R(x), R(y), C_WHITE, (int(animTick)%8 < 4)?'O':'o');
    }
    
}

bool Slime::receiveAttack(Entity* attacker, int damage, int type){
    if(hp > 0){
        if(type == ATTACK_TYPE_STUN && !stunned){
            stun();
            animBig = (int(animTick)%8 < 4);
        }
        hp -= damage;
        return true;
    }
    return false;
}
