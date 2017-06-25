#include "area.hpp"

#include "entity.hpp"
#include "player.hpp"


void Area::drawTiles(){
    for(int j=0;j<areaH;j++){
        for(int i=0;i<areaW;i++){
            Tile tile = tileAt(i, j);
            putCharA(i, j, tile.color, tile.symbol);
        }
    }
}

void Area::update(){

    drawTiles();
    /*for(int i=0;i<portalCount;i++){
        putCharA(AREA_X(portals[i].pos), AREA_Y(portals[i].pos), C_WHITE, portals[i].id);
    }*/
    
    for(size_t i=0;i<entities.size();i++){
        if(entities[i]->mark == MARK_REMOVE_NOT_DELETE){
            entities.erase(entities.begin()+i);
            i--;
        }else if(entities[i]->mark == MARK_DELETE){
            delete entities[i];
            entities.erase(entities.begin()+i);
            i--;
        }else if(entities[i] != activePlayer){
            entities[i]->update();
        }
        
    }
    activePlayer->update();
    onUpdate();
}

void Area::enter(char sym){
    activeSave->playerX = R(activePlayer->x);
    activeSave->playerY = R(activePlayer->y);
    activeSave->areaIndex = index;
    boomerangOut = false;
    entities.push_back(activePlayer);
    drawTiles();
    onEnter(sym);
}

void Area::leave(char sym){
    onLeave(sym);
    
    for(Entity* e : entities){
        if(e != activePlayer){
            delete e;
        }
    }
    entities.clear();
}


bool Area::isPlaceSolid(int x, int y, Entity* from, bool(*isEntitySolid)(Entity*)){
    if(tileAt(x, y).isSolid()){
        return true;
    }else{
        for(Entity* e : entities){
            if(e != from && x == R(e->x) && y == R(e->y) && isEntitySolid(e)){
                return true;
            }
        }
    }
    return false;
}

bool Area::attackPlace(int x, int y, Entity* attacker, int damage, int type, bool(*isEntityTarget)(Entity*)){
    bool hitSomething = tileAt(x, y).receiveAttack(damage, type);
    
    for(Entity* e : entities){
        if(e != attacker && x == R(e->x) && y == R(e->y) && isEntityTarget(e)){
            Living* l = dynamic_cast<Living*>(e);
            if(l){
                hitSomething |= l->receiveAttack(attacker, damage, type);
            }
        }
    }
    return hitSomething;
}



