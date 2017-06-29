#ifndef dialog_hpp
#define dialog_hpp

#include "global.hpp"

#include <string>
#include <vector>

#define DIALOG_STYLE_BOTTOM 0
#define DIALOG_STYLE_FULL 1

using namespace std;

class Dialog{
public:
    
    Dialog(unsigned char style, vector<string> pages) : style(style), pages(pages){}
    Dialog(vector<string> pages) : style(DIALOG_STYLE_BOTTOM), pages(pages){}
    
    string speaker = "";
    bool shouldBeDeleted = true;
    unsigned char style;
    vector<string> pages;
    int page = 0;
    float scroll = 0;
    bool waiting = false;
    
    void update();
    
};



#endif
