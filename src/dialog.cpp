#include "dialog.hpp"

#define DIALOG_SCROLL_SPEED 20

void Dialog::update(){
    
    scroll += delta * DIALOG_SCROLL_SPEED;
    
    int w = consoleW - 2;
    int h = 6;
    int diaX = 1;
    int diaY = (style == DIALOG_STYLE_FULL?1:consoleHH-h);
    
    drawFill(0, consoleH-h, consoleW, h, C_WHITE, ' ');
    
    if(speaker.size() > 0){
        putStr(diaX+(w/2)-(speaker.size()/2), diaY+consoleHH, C_WHITE, speaker.c_str());
        diaY++;
    }
    
    size_t charI = 0;
    int lastSpace = 0;
    int extra = 0;
    
    for(int printI=0;printI<scroll+extra||waiting;printI++){
        if(pages[page][charI] == ' '){
            lastSpace = charI;
        }
        if(charI > 0 && printI % w == 0 && (pages[page][charI] != ' ' && pages[page][charI-1] != ' ')){
            int printITemp = printI;
            for(int j = charI; j >= lastSpace; j--){
                int x = (printITemp % w) + diaX;
                int y = (printITemp / w) + diaY;
                putCharA(x, y, C_WHITE, ' ');
                printITemp--;
                extra++;
            }
            charI = lastSpace;
            printI--;
        }
        if(printI % w == 0 && pages[page][charI] == ' '){
            charI++;
        }
        int x = (printI % w) + diaX;
        int y = (printI / w) + diaY;
        putCharA(x, y, C_WHITE, pages[page][charI]);
        charI++;
        if(charI >= pages[page].size()){
            waiting = true;
            break;
        }
    }
    
    if(waiting){
        putCharA(consoleW-2, consoleHH-1, C_WHITE, int(scroll)%12<6?' ':'>');
    }
    if(keysJustDown[K_A] || keysJustDown[K_B] || keysJustDown[K_X] || keysJustDown[K_Y]){
        if(waiting){
            page++;
            scroll = 0;
            waiting = false;
            if(page >= pages.size()){
                page = 0;
                closeDialog();
            }
        }else{
            waiting = true;
        }
    }
    
}

