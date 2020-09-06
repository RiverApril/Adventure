#ifdef FROM_MAIN_CPP

#include <SDL.h>
#include <SDL_image.h>
#include <ctime>

#include "global.hpp"

    
const int colors[] = {
    0x000000,//black
    0x800000,//dark red
    0x008000,//dark green
    0x808000,//dark yellow
    0x0055C1,//dark blue
    0x800080,//dark magenta
    0x008080,//dark cyan
    0xAAAAAA,//light grey
    0x555555,//dark grey
    0xFF0000,//light red
    0x00FF00,//light green
    0xFFFF00,//light yellow
    0x00A9FF,//light blue
    0xFF00FF,//light magenta
    0x00FFFF,//light cyan
    0xFFFFFF,//white
};

SDL_Window* mainWindow;
SDL_Renderer* mainRenderer;
SDL_Texture* fontTexture;

char charBuffer[consoleW*consoleH] = {' '};
unsigned char colorBuffer[consoleW*consoleH] = {0};

bool running = false;

const Uint8 *keyboardState;

bool keysLast[14] = {0};
bool keysNow[14] = {0};


unsigned int begin = SDL_GetTicks();
unsigned int end;

bool initSDL(){

    debug("Init SDL\n");
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        debug("Failed to initalize SDL. Error: %s\n", SDL_GetError());
        return 1;
    }
    mainWindow = SDL_CreateWindow("Adventure", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, consoleW*fontW, consoleH*fontH, SDL_WINDOW_SHOWN);
    if(mainWindow == NULL){
        debug("Failed to create window. Error: %s\n", SDL_GetError());
        return 1;
    }
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags) ){
        debug("Failed to initalize SDL Image. Error: %s\n", IMG_GetError());
        return 1;
    }

    mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
    if(!mainRenderer){
        debug("SDL_CreateRenderer: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetSwapInterval(1);
    
    keyboardState = SDL_GetKeyboardState(NULL);

    fontTexture = IMG_LoadTexture(mainRenderer, "gfx/font.png");
    return 0;
}

void putChar(int x, int y, unsigned char color, char c){
    if(x < 0 || y < 0 || x >= consoleW || y >= consoleH){
        debug("off screen: %d, %d\n", x, y);
        return;
    }else if(c < ' '){
        debug("bad char: %d\n", c);
        return;
    }
    charBuffer[y*consoleW+x] = c;
    colorBuffer[y*consoleW+x] = color;
}

void debug(const char* fmt, ...){
    va_list args;
    va_start(args, fmt);
    printf(fmt, args);
    va_end(args);
}

void refresh(){
    SDL_RenderClear(mainRenderer);

    SDL_Rect src;
    SDL_Rect dst;

    src.w = fontW;
    src.h = fontH;

    dst.w = fontW;
    dst.h = fontH;

    for(int j=0;j<consoleH;j++){
        for(int i=0;i<consoleW;i++){

            char c = charBuffer[j*consoleW+i];
            if(c < ' '){
                continue;
            }
            unsigned char color = colorBuffer[j*consoleW+i];
            int ci = c-32;

            src.x = (ci%16)*fontW;
            src.y = (ci/16)*fontH;

            dst.x = i*fontW;
            dst.y = j*fontH;

            if(color > 0){
                SDL_SetTextureColorMod(fontTexture, (colors[color] >> 16) & 0xFF, (colors[color] >> 8) & 0xFF, (colors[color] >> 0) & 0xFF);
                SDL_RenderCopy(mainRenderer, fontTexture, &src, &dst);
            }
        }
    }

    SDL_RenderPresent(mainRenderer);

    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_QUIT:{
                running = false;
                return;
            }
        }
    }

    for(int i=0;i<14;i++){
        keysLast[i] = keysNow[i];
    }

    keysNow[K_A] = keyboardState[SDL_SCANCODE_A];
    keysNow[K_B] = keyboardState[SDL_SCANCODE_S];
    keysNow[K_X] = keyboardState[SDL_SCANCODE_X];
    keysNow[K_Y] = keyboardState[SDL_SCANCODE_Z];
    keysNow[K_SELECT] = keyboardState[SDL_SCANCODE_TAB];
    keysNow[K_START] = keyboardState[SDL_SCANCODE_RETURN];
    keysNow[K_L] = keyboardState[SDL_SCANCODE_Q];
    keysNow[K_R] = keyboardState[SDL_SCANCODE_W];
    keysNow[K_LEFT] = keyboardState[SDL_SCANCODE_LEFT];
    keysNow[K_RIGHT] = keyboardState[SDL_SCANCODE_RIGHT];
    keysNow[K_UP] = keyboardState[SDL_SCANCODE_UP];
    keysNow[K_DOWN] = keyboardState[SDL_SCANCODE_DOWN];
    keysNow[K_TOUCH] = keyboardState[SDL_SCANCODE_SPACE];
    keysNow[K_LID] = keyboardState[SDL_SCANCODE_ESCAPE];
    
    end = SDL_GetTicks();
    delta = double(end - begin) / 1000.0;
    begin = SDL_GetTicks();

}

int main(){

    if(initSDL()){
        return 1;
    }

    running = init();
    
    while(running){
        if(!update()){
            running = false;
        }
    }

    cleanup();

    SDL_DestroyWindow(mainWindow);
    SDL_Quit();

    return 0;
}

#endif
