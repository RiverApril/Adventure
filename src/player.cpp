#include "player.hpp"
#include "global.hpp"

#include "area.hpp"
#include "arrow.hpp"
#include "boomerang.hpp"
#include "bomb.hpp"
#include "npc.hpp"
#include "lock.hpp"
#include "chest.hpp"

#define PLAYER_SPEED 10

bool isTargetForPlayer(Entity* e){
    CanBeHit* c = dynamic_cast<CanBeHit*>(e);
    return (c && c->shouldBeSolid());
}

bool isSolidForPlayer(Entity* e){
    CanBeHit* c = dynamic_cast<CanBeHit*>(e);
    Lock* l = dynamic_cast<Lock*>(e);
    return (c && c->shouldBeSolid()) || dynamic_cast<Npc*>(e) || dynamic_cast<Chest*>(e) || (l && l->locked);
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
    
    
    
    if(outOfSomethingFlashTimer > 0){
        outOfSomethingFlashTimer -= delta;
    }
    if(outOfKeysFlashTimer > 0){
        outOfKeysFlashTimer -= delta;
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
        switch(activeSave->sword){
            case SWORD_STICK:{
                int xx = R(x)+DIR_X(faceDir);
                int yy = R(y)+DIR_Y(faceDir);
                char cc = (faceDir == DIR_N || faceDir == DIR_S) ? '|' : '-';
                putCharA(xx, yy, swordColor(), cc);
                activeArea->attackPlace(xx, yy, this, swordDamage(), ATTACK_TYPE_NORMAL, isTargetForPlayer);
                break;
            }
            case SWORD_BASIC:{
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
                break;
            }
        }
    }
    
    if(useTimer <= 0){
        if(keysJustDown[K_Y] || keysJustDown[K_A]){
            
            Npc* npc = nullptr;
            Lock* lock = nullptr;
            Chest* chest = nullptr;
            for(Entity* e : activeArea->entities){
                if(R(x+DIR_X(dir)) == R(e->x) && R(y+DIR_Y(dir)) == R(e->y)){
                    npc = dynamic_cast<Npc*>(e);
                    lock = dynamic_cast<Lock*>(e);
                    chest = dynamic_cast<Chest*>(e);
                    if(npc || lock || chest){
                        break;
                    }
                }
            }
            if(npc){
                activeDialog = npc->dialog;
                
            }else if(lock){
                if(lock->keyable){
                    if(activeSave->keyCount > 0){
                        activeSave->keyCount--;
                        lock->locked = false;
                    }else{
                        outOfKeysFlashTimer = OUT_OF_SOMETHING_FLASH_TIMER_MAX;
                    }
                }else{
                    activeDialog = new Dialog({"The door is locked but there's no place for a key."});
                }
            }else if(chest){
                chest->open();
                
            }else{
                switch (activeSave->sword) {
                    case SWORD_STICK:{
                        if(swingCooldownTimer <= 0 && swingTimer <= 0){
                            swingTimer = STAB_TIMER_MAX;
                            swingCooldownTimer = SWING_COOLDOWN_TIMER_MAX+STAB_TIMER_MAX;
                            faceDir = dir;
                        }
                        break;
                    }
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
        }
        
        if(keysJustDown[K_X] || keysJustDown[K_B]){
            switch (activeSave->useSelected) {
                case USE_BOW:{
                    if(activeSave->arrowCount > 0){
                        Arrow* arrow = new Arrow(R(x)+DIR_X(dir), R(y)+DIR_Y(dir), dir);
                        activeArea->entities.push_back(arrow);
                        activeSave->arrowCount--;
                    }else{
                        outOfSomethingFlashTimer = OUT_OF_SOMETHING_FLASH_TIMER_MAX;
                    }
                    useTimer = USE_BOW_TIMER_MAX;
                    faceDir = dir;
                    break;
                }
                case USE_BOOMERANG:{
                    if(!boomerangOut){
                        boomerangOut = true;
                        Boomerang* boomerang = new Boomerang(R(x)+DIR_X(dir), R(y)+DIR_Y(dir), dir);
                        activeArea->entities.push_back(boomerang);
                    }
                    break;
                }
                case USE_BOMB:{
                    if(activeSave->bombCount > 0){
                        Bomb* bomb = new Bomb(R(x)+DIR_X(dir), R(y)+DIR_Y(dir));
                        activeArea->entities.push_back(bomb);
                        activeSave->bombCount--;
                    }else{
                        outOfSomethingFlashTimer = OUT_OF_SOMETHING_FLASH_TIMER_MAX;
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
        //putCharA(R(x), R(y)-1, C_WHITE, 'o');
        //putCharA(R(x), R(y), C_WHITE, (tryToMove && (int(animTick) % 2 == 0))?(DIR_Y(dir)==0?'X':'Y'):(DIR_Y(dir)==0?'Y':'X'));
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

