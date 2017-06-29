
#include "entity.hpp"

bool CanBeHit::shouldBeSolid(){
    Living* l = dynamic_cast<Living*>(this);
    return (!l) || (l && l->hp > 0);
}
