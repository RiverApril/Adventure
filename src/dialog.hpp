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
    
    Dialog(bool shouldBeDeleted, unsigned char style, vector<string> pages) : shouldBeDeleted(shouldBeDeleted), style(style), pages(pages){}
    
    bool shouldBeDeleted;
    unsigned char style;
    vector<string> pages;
    int page = 0;
    float scroll = 0;
    bool waiting = false;
    
    void update();
    
};



#endif
