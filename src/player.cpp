#include "player.hpp"
#include "global.hpp"

#include "area.hpp"
#include "arrow.hpp"
#include "boomerang.hpp"

#define PLAYER_SPEED 10

bool isTargetForPlayer(Entity* e){
    return dynamic_cast<Living*>(e);
}

bool isSolidForPlayer(Entity* e){
    return false;
}

void Player::update(){
    Entity::update();

    const float spd = delta * PLAYER_SPEED;

    float dx = 0;
    float dy = 0;
    bool tryToMove = false;

    if(keysNow[K_LEFT]){
        dir = DIR_W;
        dx -= spd;
        tryToMove = true;
    }
    if(keysNow[K_RIGHT]){
        dir = DIR_E;
        dx += spd;
        tryToMove = true;
    }
    if(keysNow[K_UP]){
        dir = DIR_N;
        dy -= spd;
        tryToMove = true;
    }
    if(keysNow[K_DOWN]){
        dir = DIR_S;
        dy += spd;
        tryToMove = true;
    }

    if(keysNow[K_L]){ // cheat code
        x += dx;
        y += dy;
    }else{
        if(!activeArea->isPlaceSolid(R(x+dx), R(y), this, isSolidForPlayer)){
            x += dx;
        }
        if(!activeArea->isPlaceSolid(R(x), R(y+dy), this, isSolidForPlayer)) {
            y += dy;
        }
    }

    if(!tryToMove){
        x = R(x);
        y = R(y);
    }
    
    
    
    if(keysJustDown[K_Y]){
        
    }
    
    if(swingCooldownTimer > 0){
        swingCooldownTimer -= delta;
    }
    if(useTimer > 0){
        useTimer -= delta;
        switch (activeSave->useSelected) {
            case USE_BOW:{
                if(DIR_X(faceDir)){
                    putCharA(R(x+DIR_X(faceDir)), R(y+DIR_Y(faceDir)), useColor(), faceDir==DIR_E?')':'(');
                }
                break;
            }
        }
    }
    if(swingTimer > 0){
        swingTimer -= delta;
        int xx = 0;
        int yy = 0;
        char cc;
        if(swingTimer > (SWING_TIMER_MAX / 3 * 2)){
            xx += (faceDir == DIR_E || faceDir == DIR_S) ? 1 : -1;
            yy += (faceDir == DIR_W || faceDir == DIR_S) ? 1 : -1;
            cc = (faceDir == DIR_E || faceDir == DIR_W) ? '/' : '\\';
        } else if(swingTimer > (SWING_TIMER_MAX / 3)){
            xx += (faceDir == DIR_E) ? 1 : ((faceDir == DIR_W) ? -1 : 0);
            yy += (faceDir == DIR_S) ? 1 : ((faceDir == DIR_N) ? -1 : 0);
            cc = (faceDir == DIR_N || faceDir == DIR_S) ? '|' : '-';
            char ccc = (faceDir == DIR_N) ? ',' : ((faceDir == DIR_S) ? '\'' : '-');
            int xxx = R(x)+xx*2;
            int yyy = R(y)+yy*2;
            putCharA(xxx, yyy, swordColor(), ccc);
            activeArea->attackPlace(xxx, yyy, this, swordDamage(), ATTACK_TYPE_NORMAL, isTargetForPlayer);
        } else {
            xx += (faceDir == DIR_E || faceDir == DIR_N) ? 1 : -1;
            yy += (faceDir == DIR_E || faceDir == DIR_S) ? 1 : -1;
            cc = (faceDir == DIR_N || faceDir == DIR_S) ? '/' : '\\';
        }
        xx += R(x);
        yy += R(y);
        putCharA(xx, yy, swordColor(), cc);
        activeArea->attackPlace(xx, yy, this, swordDamage(), ATTACK_TYPE_NORMAL, isTargetForPlayer);
    }
    
    if(useTimer <= 0){
        if(keysJustDown[K_Y] || keysJustDown[K_A]){
            switch (activeSave->sword) {
                case SWORD_BASIC:{
                    if(swingCooldownTimer <= 0 && swingTimer <= 0){
                        swingTimer = SWING_TIMER_MAX;
                        swingCooldownTimer = SWING_COOLDOWN_TIMER_MAX+SWING_TIMER_MAX;
                        faceDir = dir;
                    }
                    break;
                }
            }
        }
        
        if(keysJustDown[K_X] || keysJustDown[K_B]){
            switch (activeSave->useSelected) {
                case USE_BOW:{
                    if(activeSave->arrowCount > 0){
                        Arrow* arrow = new Arrow(R(x)+DIR_X(dir), R(y)+DIR_Y(dir), dir);
                        activeArea->entities.push_back(arrow);
                        activeSave->arrowCount--;
                    }
                    useTimer = USE_BOW_TIMER_MAX;
                    faceDir = dir;
                    break;
                }
                case USE_BOOMERANG:{
                    if(!activeArea->boomerangOut){
                        activeArea->boomerangOut = true;
                        Boomerang* boomerang = new Boomerang(R(x)+DIR_X(dir), R(y)+DIR_Y(dir), dir);
                        activeArea->entities.push_back(boomerang);
                    }
                    break;
                }
            }
        }
    }
    
    
    
    for(int i = 0; i < activeArea->portalCount; i++){
        int pos = activeArea->portals[i].pos;
        if(pos == AREA_POS(R(x), R(y)) ||
           (pos == PORTAL_POS_E && R(x) == areaW) ||
           (pos == PORTAL_POS_W && R(x) == -1) ||
           (pos == PORTAL_POS_S && R(y) == areaH) ||
           (pos == PORTAL_POS_N && R(y) == -1)){
            moveAreas(i);
            break;
        }
    }

    if(invulnerableTimer > 0){
        invulnerableTimer -= delta;
        putCharA(R(x), R(y), int(animTick) % 2 == 0 ?C_WHITE:C_DARK_GREY, '@');
    }else{
        putCharA(R(x), R(y), C_WHITE, '@');
    }
    

}


bool Player::receiveAttack(Entity* attacker, int damage, int type){
    if(invulnerableTimer <= 0){
        activeSave->hp -= damage;
        invulnerableTimer = INVULNERABLE_TIMER_MAX;
        return true;
    }
    return false;
}
