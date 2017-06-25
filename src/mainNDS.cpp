#ifdef FROM_MAIN_CPP

#include <nds.h>
#include <fat.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>

#include "global.hpp"

#include "font.h"

#define FPS 60

// NDS screen: 256 x 192
// Font: 7 x 12
// h count: 256/7 ~= 36.57, no more than 36
// v count: 192/12 = 16.0
// so viewable tiles: 36 x 16

int bgMain, bgSub;
u8 screenBitmapMain[256*consoleH*fontH] = {0};
u8 screenBitmapSub[256*consoleH*fontH] = {0};

bool keysLast[14] = {0};
bool keysNow[14] = {0};

bool running = false;
 

u16 colorPal[32] = {
    (u16)RGB15(0, 0, 0),
    (u16)RGB15(16, 0, 0),
    (u16)RGB15(0, 16, 0),
    (u16)RGB15(16, 16, 0),
    (u16)RGB15(0, 8, 24),
    (u16)RGB15(16, 0, 16),
    (u16)RGB15(0, 16, 16),
    (u16)RGB15(24, 24, 24),
    (u16)RGB15(8, 8, 8),
    (u16)RGB15(31, 0, 0),
    (u16)RGB15(0, 31, 0),
    (u16)RGB15(31, 31, 0),
    (u16)RGB15(0, 8, 31),
    (u16)RGB15(31, 0, 31),
    (u16)RGB15(0, 31, 31),
    (u16)RGB15(31, 31, 31),

    (u16)RGB15(0, 0, 0),
    (u16)RGB15(8, 0, 0),
    (u16)RGB15(0, 8, 0),
    (u16)RGB15(8, 8, 0),
    (u16)RGB15(0, 4, 12),
    (u16)RGB15(8, 0, 8),
    (u16)RGB15(0, 8, 8),
    (u16)RGB15(12, 12, 12),
    (u16)RGB15(4, 4, 4),
    (u16)RGB15(16, 0, 0),
    (u16)RGB15(0, 16, 0),
    (u16)RGB15(16, 16, 0),
    (u16)RGB15(0, 4, 16),
    (u16)RGB15(16, 0, 16),
    (u16)RGB15(0, 16, 16),
    (u16)RGB15(16, 16, 16)
};

void putChar(int x, int y, u8 color, char c){
    if(x > consoleW || y > consoleH){
        debug("off screen: %d, %d\n", x, y);
        return;
    }else if(c < ' '){
        debug("bad char: %d\n", c);
        return;
    }

    u8* scr = screenBitmapMain;

    u8 yy = y;

    if(yy >= consoleHH){
        yy -= consoleHH;
        scr = screenBitmapSub;
    }

    const u8 cPos = c-32;
    const u8 cCol = cPos%16;
    const u8 cRow = cPos/16;
    const u8 sPixX = cCol*fontW;
    const u8 sPixY = cRow*fontH;
    const u16 sPos = (u16)sPixY*(fontW*16)+(u16)sPixX;

    const u8 dPixX = x*fontW;
    const u8 dPixY = yy*fontH;
    const u16 dPos = (u16)dPixY*256+(u16)dPixX;

    int fj = 0;
    for(int j = 0; j < fontH*256; j += 256){
        fj += (fontW*16);
        for(int i = 0; i < fontW; i++){
            u8 sPix = ((u8*)fontBitmap)[sPos + fj + i];
            if(sPix == 2){ // whatever white is
                scr[dPos+j+i] = color;
            }else if(sPix == 1){ // whatever grey is
                scr[dPos+j+i] = color+16;
            }else{ // whatever black is
                scr[dPos+j+i] = 0;
            }
        }
        //memcpy(screenBitmap+dPos+j, ((u8*)fontBitmap)+sPos+j, fontW);
    }
}

void refresh(){
    dmaCopy(screenBitmapMain, bgGetGfxPtr(bgMain), 256*consoleH*fontH);
    dmaCopy(screenBitmapSub, bgGetGfxPtr(bgSub), 256*consoleH*fontH);
    swiWaitForVBlank();
}

void debug(const char* fmt, ...){
    va_list args;
    va_start(args, fmt);
    printf(fmt, args);
    va_end(args);
}

int main(void) {

    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    videoSetModeSub(MODE_5_2D);
    vramSetBankC(VRAM_C_SUB_BG);

    //consoleDemoInit();

    bgMain = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    bgSub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

    //dmaCopy(screenBitmap, bgGetGfxPtr(bgMain), 256*256);
    //dmaCopy(screenBitmap, bgGetGfxPtr(bgSub), 256*256);
    dmaCopy(colorPal, BG_PALETTE, 32*2);
    dmaCopy(colorPal, BG_PALETTE_SUB, 32*2);


    swiWaitForVBlank();

    if(!fatInitDefault()){
        debug("fatInitDefault failed\n");
    }

    cpuStartTiming(0);

    u32 begin = cpuGetTiming();
    u32 end;

    float delta;

    running = init();

    while(running){
        end = cpuGetTiming();
        delta = float(end-begin) / BUS_CLOCK;
        begin = end;

        scanKeys();
        int keys = keysHeld();
        for(int i=0;i<14;i++){
            keysLast[i] = keysNow[i];
            keysNow[i] = keys & (1 << i);
        }

        running = update(delta);
    }

    cleanup();

    return 0;
}

#endif
