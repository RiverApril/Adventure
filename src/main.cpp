#define FROM_MAIN_CPP

#ifdef ARM9
#include "mainNDS.cpp"

#define STR_K_A "A"
#define STR_K_B "B"
#define STR_K_SELECT "SELECT"
#define STR_K_START "START"
#define STR_K_RIGHT "RIGHT"
#define STR_K_LEFT "LEFT"
#define STR_K_UP "UP"
#define STR_K_DOWN "DOWN"
#define STR_K_R "R"
#define STR_K_L "L"
#define STR_K_X "X"
#define STR_K_Y "Y"
#define STR_K_TOUCH "TOUCH"
#define STR_K_LID "LID"

#else
#include "mainSDL.cpp"

#define STR_K_A "A"
#define STR_K_B "S"
#define STR_K_SELECT "TAB"
#define STR_K_START "ENTER"
#define STR_K_RIGHT "RIGHT"
#define STR_K_LEFT "LEFT"
#define STR_K_UP "UP"
#define STR_K_DOWN "DOWN"
#define STR_K_R "W"
#define STR_K_L "Q"
#define STR_K_X "X"
#define STR_K_Y "Z"
#define STR_K_TOUCH "SPACE"
#define STR_K_LID "ESC"

#endif

#include "gen/areas.hpp"
#include "player.hpp"
#include "dialog.hpp"


void putStrJustS(const unsigned char x, const unsigned char y, const unsigned char color, const char* s){
    int n = 0;
    char c;
    while((c = s[n]) != '\0'){
        putChar(x+n, y, color, c);
        n++;
    }
}

void putStr(const unsigned char x, const unsigned char y, const unsigned char color, const char* fmt, ...){
    unsigned char n = consoleW+1;
    char buf[n];

    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    putStrJustS(x, y, color, buf);
}

bool keysJustDown[14];
bool keysJustUp[14];

SaveData allSaves[3];
SaveData* activeSave;

float delta = 0;
float secondsElapsed = 0;

Area* activeArea;
Player* activePlayer;
Dialog* activeDialog;

bool boomerangOut;

bool loadGame(){

    FILE* fp = fopen("Adventure.sav", "rb");
    if(fp){
        size_t worked = fread(allSaves, sizeof(allSaves), 1, fp);
        fclose(fp);
        if(worked){
            debug("Read from sav.\n");
        }else{
            debug("Faield to read from sav.\n");
        }
        return worked;
    }else{
        debug("Failed to open sav for loading.\n");
    }
    
    return false;
}

bool saveGame(){
    FILE* fp = fopen("Adventure.sav", "wb");
    if(fp){
        size_t worked = fwrite(allSaves, sizeof(allSaves), 1, fp);
        fclose(fp);
        if(worked){
            debug("Wrote to sav.\n");
        }else{
            debug("Faield to write to sav.\n");
        }
        return worked;
    }else{
        debug("Failed to open sav writing.\n");
    }
    return false;
}

bool init(){
    
    loadGame();
    
    return true;
}

void startGame(int i){
    
    activeSave = &allSaves[i];
    
    if(!activeSave->started){
        activeSave->started = true;
        activeSave->hp = 6;
        activeSave->maxHp = 6;
        activeSave->spawnHp = 6;
        activeSave->playerX = 21; // only set these when entering a room
        activeSave->playerY = 6; // ''
        activeSave->areaIndex = areaInsideHome->index;
        activeSave->playerSpawnX = 21;
        activeSave->playerSpawnY = 6;
        activeSave->spawnAreaIndex = areaInsideHome->index;
        for(int i=0;i<USE_COUNT;i++){
            activeSave->useUnlocked[i] = false;//rand()%2;
        }
        activeSave->useUnlocked[USE_BOW] = true;
        activeSave->useUnlocked[USE_BOOMERANG] = true;
        activeSave->useUnlocked[USE_BOMB] = true;
        activeSave->useSelected = USE_NONE;
        activeSave->sword = SWORD_BASIC;
        activeSave->keyCount = 0;
        activeSave->arrowCount = 20;
        activeSave->bombCount = 20;
        activeSave->areaData = AreaSaveData();
    }

    activeDialog = nullptr;
    
    activeArea = allAreas[activeSave->areaIndex];
    
    activePlayer = new Player(activeSave->playerX, activeSave->playerY);
    
    activeArea->enter('?');
    saveGame();
}

void setSpawnPoint(unsigned char x, unsigned char y, int i){
    activeSave->playerSpawnX = x;
    activeSave->playerSpawnY = y;
    activeSave->spawnAreaIndex = i;
}

#define MENU_MAIN_MENU 0
#define MENU_GAME 1
#define MENU_INV 2
#define MENU_PAUSE 3
#define MENU_GAME_OVER 4
#define MENU_SAVED 5

int menu = MENU_MAIN_MENU;
int mainSelected = 0;

void putCharA(int x, int y, unsigned char color, char c){
    putChar(x, y+consoleHH, color, c);
}

void drawBox(int x, int y, int w, int h){
    putChar(x, y, C_WHITE, '+');
    putChar(x+w-1, y, C_WHITE, '+');
    putChar(x, y+h-1, C_WHITE, '+');
    putChar(x+w-1, y+h-1, C_WHITE, '+');
    
    for(int i=x+1;i<x+w-1;i++){
        putChar(i, y, C_WHITE, '-');
        putChar(i, y+h-1, C_WHITE, '-');
    }
    for(int i=y+1;i<y+h-1;i++){
        putChar(x, i, C_WHITE, '|');
        putChar(x+w-1, i, C_WHITE, '|');
    }
}
void drawFill(int x, int y, int w, int h, unsigned char color, char c){
    for(int i=x; i<x+w; i++){
        for(int j=y; j<y+h; j++){
            putChar(i, j, color, c);
        }
    }
}

void closeDialog(){
    if(activeDialog){
        if(activeDialog->shouldBeDeleted){
            delete activeDialog;
        }
        activeDialog = nullptr;
    }
}

bool update(){
    secondsElapsed += delta;

    for(int i=0;i<14;i++){
        keysJustDown[i] = !keysLast[i] && keysNow[i];
        keysJustUp[i] = keysLast[i] && !keysNow[i];
    }
    
    if(activeDialog){
        activeDialog->update();
    }else{
    
        if(menu == MENU_MAIN_MENU){
            drawFill(0, 0, consoleW, consoleH, C_WHITE, ' ');
            putStr(2, consoleHH+1, C_WHITE, "A D V E N T U R E");
            for(int i=0;i<SAVE_COUNT;i++){
                putStr(4, consoleHH+3+i, C_WHITE, "%c %s Slot %d", mainSelected == i ? '-' : ' ', allSaves[i].started?"Resume":"Begin", i+1);
            }
            putStr(2, consoleHH+6+SAVE_COUNT, C_WHITE, "[" STR_K_START "]: play");
            putStr(2, consoleHH+7+SAVE_COUNT, C_WHITE, "[" STR_K_L "]+[" STR_K_R "]+[" STR_K_START "]: delete");
            
            if(keysNow[K_L] && keysNow[K_R] && keysJustDown[K_START]){ // delete
                allSaves[mainSelected].started = false;
                saveGame();
            }else if(keysNow[K_X] && keysNow[K_Y] && keysJustDown[K_START]){ // cheat recover code
                allSaves[mainSelected].started = true;
                saveGame();
            }else if(keysJustDown[K_START] || keysJustDown[K_A]){ // start
                startGame(mainSelected);
                menu = MENU_GAME;
            }
            if(keysJustDown[K_UP]){
                mainSelected--;
                if(mainSelected < 0){
                    mainSelected = SAVE_COUNT-1;
                }
            }
            if(keysJustDown[K_DOWN]){
                mainSelected++;
                if(mainSelected >= SAVE_COUNT){
                    mainSelected = 0;
                }
            }
        }else{
        
            switch(menu){
                case MENU_GAME:{
                    if(keysJustDown[K_SELECT]){
                        menu = MENU_INV;
                    }
                    if(keysJustDown[K_START]){
                        menu = MENU_PAUSE;
                    }
                    if(activeSave->hp <= 0){
                        menu = MENU_GAME_OVER;
                    }
                    activeArea->update();
                    break;
                }
                case MENU_INV:{
                    if(keysJustDown[K_SELECT] || keysJustDown[K_A] || keysJustDown[K_B] || keysJustDown[K_X] || keysJustDown[K_Y]){
                        menu = MENU_GAME;
                    }
                    drawFill(0, consoleHH, consoleW, consoleHH, C_WHITE, ' ');
                    const int invHC = 6;
                    const int invVC = 6;
                    const int invX = 1;
                    const int invY = 2;
                    const int invW = invHC*2+3;
                    const int invH = invVC+2;
                    putStr(consoleW/2-9, consoleHH, C_WHITE, " [" STR_K_SELECT "]: resume ");
                    drawBox(invX, invY+consoleHH, invW, invH);
                    for(int i = 0; i < invHC; i++){
                        for(int j = 0; j < invVC; j++){
                            int use = j*invHC+i;
                            if(use < USE_COUNT && activeSave->useUnlocked[use]){
                                putCharA(invX+(i*2)+2, invY+j+1, useColor(use), useChar(use));
                            }
                        }
                    }
                    drawBox(consoleW-18, invY+consoleHH, 17, 5);
                    putStr(consoleW-11, invY+consoleHH+1, C_WHITE, "[ ]");
                    putChar(consoleW-10, invY+consoleHH+1, useColor(activeSave->useSelected), useChar(activeSave->useSelected));
                    putStr(consoleW-10-(strlen(useName(activeSave->useSelected))/2), invY+consoleHH+3, C_WHITE, useName(activeSave->useSelected));
                    if(activeSave->useSelected == USE_NONE){
                        for(int i=0;i<USE_COUNT;i++){
                            if(activeSave->useUnlocked[i]){
                                activeSave->useSelected = i;
                                break;
                            }
                        }
                    }
                    if(activeSave->useSelected != USE_NONE){
                        int i = activeSave->useSelected % invHC;
                        int j = activeSave->useSelected / invHC;
                        putCharA(invX+(i*2)+1, invY+j+1, C_WHITE, '[');
                        putCharA(invX+(i*2)+3, invY+j+1, C_WHITE, ']');
                        if(keysJustDown[K_LEFT]){
                            do{
                                activeSave->useSelected--;
                                if(activeSave->useSelected < 0){
                                    activeSave->useSelected = USE_COUNT-1;
                                }
                            }while(!activeSave->useUnlocked[activeSave->useSelected]);
                        }
                        if(keysJustDown[K_RIGHT]){
                            do{
                                activeSave->useSelected++;
                                if(activeSave->useSelected >= USE_COUNT){
                                    activeSave->useSelected = 0;
                                }
                            }while(!activeSave->useUnlocked[activeSave->useSelected]);
                        }
                        if(keysJustDown[K_UP]){
                            do{
                                activeSave->useSelected -= invHC;
                                if(activeSave->useSelected < 0){
                                    activeSave->useSelected += USE_COUNT;
                                }
                            }while(!activeSave->useUnlocked[activeSave->useSelected]);
                        }
                        if(keysJustDown[K_DOWN]){
                            do{
                                activeSave->useSelected += invHC;
                                if(activeSave->useSelected >= USE_COUNT){
                                    activeSave->useSelected -= USE_COUNT;
                                }
                            }while(!activeSave->useUnlocked[activeSave->useSelected]);
                        }
                    }
                    drawBox(consoleW-18, invY+consoleHH+6, 17, 3);
                    if(activeSave->sword != SWORD_NONE){
                        putChar(consoleW-16, invY+consoleHH+7, swordColor(), swordChar());
                        putStr(consoleW-14, invY+consoleHH+7, C_WHITE, swordName());
                    }
                    break;
                }
                case MENU_PAUSE:{
                    putStr(consoleW/2-10, consoleHH+consoleHH/2-3, C_LIGHT_BLUE, "                    ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2-2, C_LIGHT_BLUE, "    P A U S E D     ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2-1, C_LIGHT_BLUE, "                    ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2+0, C_LIGHT_BLUE, "     [" STR_K_A "]: save      ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2+1, C_LIGHT_BLUE, "                    ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2+2, C_LIGHT_BLUE, "  [" STR_K_START "]: resume   ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2+3, C_LIGHT_BLUE, "                    ");
                    if(keysJustDown[K_START]){
                        menu = MENU_GAME;
                    }
                    if(keysJustDown[K_A]){
                        putStr(consoleW/2-10, consoleHH+consoleHH/2+0, C_LIGHT_BLUE, "     Saving...      ");
                        refresh();
                        saveGame();
                        menu = MENU_SAVED;
                    }
                    break;
                }
                case MENU_GAME_OVER:{
                    putStr(consoleW/2-10, consoleHH+consoleHH/2-3, C_LIGHT_RED, "                    ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2-2, C_LIGHT_RED, " G A M E    O V E R ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2-1, C_LIGHT_RED, "                    ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2+0, C_LIGHT_RED, " [" STR_K_START "]: try again ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2+1, C_LIGHT_RED, "                    ");
                    if(keysJustDown[K_START]){
                        menu = MENU_GAME;
                        activeArea->leave('?');
                        activeArea = allAreas[activeSave->spawnAreaIndex];
                        activeSave->hp = activeSave->spawnHp;
                        activePlayer->x = activeSave->playerSpawnX;
                        activePlayer->y = activeSave->playerSpawnY;
                        activePlayer->invulnerableTimer = 0;
                        activeArea->enter('?');
                    }
                    break;
                }
                case MENU_SAVED:{
                    putStr(consoleW/2-10, consoleHH+consoleHH/2-3, C_LIGHT_BLUE, "                    ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2-2, C_LIGHT_BLUE, "     S A V E D      ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2-1, C_LIGHT_BLUE, "                    ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2+0, C_LIGHT_BLUE, "     [" STR_K_A "]: quit      ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2+1, C_LIGHT_BLUE, "                    ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2+2, C_LIGHT_BLUE, "  [" STR_K_START "]: resume   ");
                    putStr(consoleW/2-10, consoleHH+consoleHH/2+3, C_LIGHT_BLUE, "                    ");
                    if(keysJustDown[K_START]){
                        menu = MENU_GAME;
                    }
                    if(keysJustDown[K_A]){
                        activeArea->leave('?');
                        delete activePlayer;
                        menu = MENU_MAIN_MENU;
                        refresh();
                        return true;
                    }
                    break;
                }
            }

            putStr(0, 0, C_WHITE, "Delta: %f", delta);
            putStr(0, 1, C_WHITE, "Time: %d", (int)round(secondsElapsed));
            putStr(0, 2, C_WHITE, "Ents: %d", (int)activeArea->entities.size());
            putStr(0, 3, C_WHITE, "Player: %d, %d ", R(activePlayer->x), R(activePlayer->y));
            
            const unsigned char menuY = consoleHH-5;
            drawBox(0, menuY, consoleW, 5);
            
            
            putStr(7, menuY+1, C_WHITE, "Life");
            bool flashHp = (activeSave->hp == 1 && int(secondsElapsed*ANIM_TICK_LENGTH)%2==0) || (activeSave->hp == 2 && int(secondsElapsed*ANIM_TICK_LENGTH)%4<2);
            for(int i=0;i<activeSave->maxHp;i+=2){
                putChar(12+i/2, menuY+1, flashHp?C_DARK_GREY:C_LIGHT_RED, activeSave->hp > i ? ((activeSave->hp-i == 1) ? '=' : '*') : '-');
            }
            
            putStr(2, menuY+1, C_WHITE, STR_K_A);
            putChar(2, menuY+2, swordColor(), swordChar());
            
            putStr(4, menuY+1, C_WHITE, STR_K_B);
            putChar(4, menuY+2, useColor(), useChar());
            
            if(activeSave->useUnlocked[USE_BOW]){
                bool flashArrows = (activeSave->useSelected == USE_BOW && int(activePlayer->outOfSomethingFlashTimer*ANIM_TICK_LENGTH)%2==1);
                putStr(7, menuY+2, flashArrows?C_DARK_GREY:C_WHITE, "Arrows");
                putStr(14, menuY+2, flashArrows?C_DARK_GREY:C_WHITE, "%d ", activeSave->arrowCount);
            }
            
            if(activeSave->useUnlocked[USE_BOMB]){
                bool flashBombs = (activeSave->useSelected == USE_BOMB && int(activePlayer->outOfSomethingFlashTimer*ANIM_TICK_LENGTH)%2==1);
                putStr(18, menuY+2, flashBombs?C_DARK_GREY:C_WHITE, "Bombs");
                putStr(24, menuY+2, flashBombs?C_DARK_GREY:C_WHITE, "%d ", activeSave->bombCount);
            }
            
            bool flashKeys = int(activePlayer->outOfKeysFlashTimer*ANIM_TICK_LENGTH)%2==1;
            putStr(7, menuY+3, flashKeys?C_DARK_GREY:C_WHITE, "Keys");
            putStr(12, menuY+3, flashKeys?C_DARK_GREY:C_WHITE, "%d ", activeSave->keyCount);
        
        }
    }

    refresh();

    return true;
}

void cleanup(){
    //pass
}

float randf(){
    return float(rand() / float(RAND_MAX));
}
unsigned char swordColor(){
    switch (activeSave->sword) {
        case SWORD_NONE:  return C_WHITE;
        case SWORD_BASIC: return C_LIGHT_GREY;
        case SWORD_STICK: return C_DARK_YELLOW;
        default: return C_WHITE;
    }
}
unsigned char swordChar(){
    switch (activeSave->sword) {
        case SWORD_NONE:  return ' ';
        case SWORD_BASIC: return '/';
        case SWORD_STICK: return '/';
        default: return '?';
    }
}
int swordDamage(){
    switch (activeSave->sword) {
        case SWORD_NONE:  return 0;
        case SWORD_BASIC: return 1;
        case SWORD_STICK: return 1;
        default: return 0;
    }
}
const char* swordName(){
    switch (activeSave->sword) {
        case SWORD_NONE:  return "Nothing";
        case SWORD_STICK: return "Stick";
        case SWORD_BASIC: return "Basic Sword";
        default: return "???";
    }
}
unsigned char useColor(int use){
    switch (use) {
        case USE_NONE:      return C_WHITE;
        case USE_BOW:       return C_LIGHT_YELLOW;
        case USE_BOOMERANG: return C_LIGHT_GREEN;
        case USE_BOMB:      return C_LIGHT_RED;
        default:            return C_WHITE;
    }
}
unsigned char useChar(int use){
    switch (use) {
        case USE_NONE:      return ' ';
        case USE_BOW:       return ')';
        case USE_BOOMERANG: return 'L';
        case USE_BOMB:      return 'o';
        default: return '?';
    }
}
const char* useName(int use){
    switch (use) {
        case USE_NONE:      return "";
        case USE_BOW:       return "Bow";
        case USE_BOOMERANG: return "Boomerang";
        case USE_BOMB:      return "Bomb";
        default: return "???";
    }
}
const char* usePickupName(int use){
    switch (use) {
        case USE_NONE:      return "";
        case USE_BOW:       return "Bow and quiver";
        case USE_BOOMERANG: return "Boomerang";
        case USE_BOMB:      return "Bomb bag";
        default: return "???";
    }
}

void moveAreas(int portalIndex){
    activeArea->leave(activeArea->portals[portalIndex].id);
    char fromId = activeArea->portals[portalIndex].id;
    Area* prevArea = activeArea;
    activeArea = prevArea->portals[portalIndex].out;

    char toId = fromId;
    int x = R(activePlayer->x);
    int y = R(activePlayer->y);
    
    switch(fromId){
        case 'S': y = 0; toId = 'N'; break;
        case 'N': y = areaH-1; toId = 'S'; break;
        case 'E': x = 0; toId = 'W'; break;
        case 'W': x = areaW-1; toId = 'E'; break;
        default:{
            for(int i = 0; i < activeArea->portalCount; i++){
                if(activeArea->portals[i].id == fromId){
                    int p = activeArea->portals[i].pos;
                    x = AREA_X(p);
                    y = AREA_Y(p);
                    if(!activeArea->tileAt(x+1, y).isSolid()){
                        x++;
                    }else if(!activeArea->tileAt(x-1, y).isSolid()){
                        x--;
                    }else if(!activeArea->tileAt(x, y+1).isSolid()){
                        y++;
                    }else if(!activeArea->tileAt(x, y-1).isSolid()){
                        y--;
                    }
                    break;
                }
            }
            break;
        }
    }
    
    activePlayer->x = x;
    activePlayer->y = y;
    
    activeArea->enter(toId);

}
