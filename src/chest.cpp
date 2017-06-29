#include "chest.hpp"
#include "global.hpp"

#include "dialog.hpp"

void Treasure::activate(){
    string text = "";
    switch (type) {
        case TREASURE_KEY:
            activeSave->keyCount++;
            text = "A small key.";
            break;
        
        case TREASURE_SWORD:
            activeSave->sword = meta;
            text = ("A "+string(swordName())+".").c_str();
            break;
            
        case TREASURE_USE:
            activeSave->useUnlocked[meta] = true;
            text = ("A "+string(usePickupName())+".").c_str();
            break;
    }
    activeDialog = new Dialog({"Inside the chest you find...", text});
}
