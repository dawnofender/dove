#include "panelThingy.hpp"

CLASS_DEFINITION(Thingy, Panel)


Panel::Panel(std::string && initialName) 
    : Thingy(std::move(initialName)) {}
