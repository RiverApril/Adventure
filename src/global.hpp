

#ifndef global_hpp
#define global_hpp

#include "areaSaveData.hpp"

#include <cmath>
#include <vector>
#include <stdlib.h>

#define C_BLACK 0
#define C_DARK_RED 1
#define C_DARK_GREEN 2
#define C_DARK_YELLOW 3
#define C_DARK_BLUE 4
#define C_DARK_MAGENTA 5
#define C_DARK_CYAN 6
#define C_LIGHT_GREY 7
#define C_LIGHT_GRAY 7
#define C_DARK_GREY 8
#define C_DARK_GRAY 8
#define C_LIGHT_RED 9
#define C_LIGHT_GREEN 10
#define C_LIGHT_YELLOW 11
#define C_LIGHT_BLUE 12
#define C_LIGHT_MAGENTA 13
#define C_LIGHT_CYAN 14
#define C_WHITE 15

#define fontW 7
#define fontH 12

#define consoleW 36
#define consoleHH 16
#define consoleH (consoleHH*2)

#define areaW consoleW
#define areaH consoleHH

#define DIR_E 0
#define DIR_N 1
#define DIR_W 2
#define DIR_S 3

#define PORTAL_POS_E (-1)
#define PORTAL_POS_N (-2)
#define PORTAL_POS_W (-3)
#define PORTAL_POS_S (-4)

#define DIR_X(d) ((d)==DIR_E?1:((d)==DIR_W?-1:0))
#define DIR_Y(d) ((d)==DIR_S?1:((d)==DIR_N?-1:0))

#define AREA_POS(x, y) ((y)*areaW+(x))
#define AREA_X(p) ((p) % areaW)
#define AREA_Y(p) ((p) / areaW)

#define K_A 0
#define K_B 1
#define K_SELECT 2
#define K_START 3
#define K_RIGHT 4
#define K_LEFT 5
#define K_UP 6
#define K_DOWN 7
#define K_R 8
#define K_L 9
#define K_X 10
#define K_Y 11
#define K_TOUCH 12
#define K_LID 13

#define USE_NONE (-1)
#define USE_BOW 0
#define USE_BOOMERANG 1
#define USE_BOMB 2
#define USE_COUNT 36

#define SWORD_NONE (-1)
#define SWORD_STICK 0
#define SWORD_BASIC 1

#define SAVE_COUNT 3

#define ANIM_TICK_LENGTH 10

extern bool keysLast[14];
extern bool keysNow[14];
extern bool keysJustDown[14];
extern bool keysJustUp[14];

struct SaveData{
    bool started = false;
    int hp;
    int maxHp;
    int spawnHp;
    unsigned char playerX;
    unsigned char playerY;
    unsigned char playerSpawnX;
    unsigned char playerSpawnY;
    int areaIndex;
    int spawnAreaIndex;
    bool useUnlocked[USE_COUNT];
    int useSelected;
    int sword;
    int keyCount;
    int arrowCount;
    int bombCount;
    AreaSaveData areaData;
};

extern SaveData allSaves[SAVE_COUNT];
extern SaveData* activeSave;

extern float delta;
extern float secondsElapsed;

struct Area;
class Player;
class Dialog;

extern Area* activeArea;
extern Player* activePlayer;
extern Dialog* activeDialog;

extern bool boomerangOut;

// Functions defined in main???.cpp
void debug(const char* fmt, ...);
void putChar(int x, int y, unsigned char color, char c);
void refresh();

// Functions called from main???.cpp
bool init(); // Returns true if should start running
bool update(float delta); // Returns true if still running
void cleanup();


float randf();
void moveAreas(int portalIndex);
void putStr(const unsigned char x, const unsigned char y, const unsigned char color, const char* fmt, ...);

void drawFill(int x, int y, int w, int h, unsigned char color, char c);
void putCharA(int x, int y, unsigned char color, char c);
unsigned char swordColor();
unsigned char swordChar();
int swordDamage();
const char* swordName();
unsigned char useColor(int use = activeSave->useSelected);
unsigned char useChar(int use = activeSave->useSelected);
const char* useName(int use = activeSave->useSelected);
const char* usePickupName(int use = activeSave->useSelected);
void closeDialog();



//


#endif
