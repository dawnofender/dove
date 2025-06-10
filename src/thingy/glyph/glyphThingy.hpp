#ifndef GLYPHTHINGY_HPP
#define GLYPHTHINGY_HPP

#include <src/thingy/thingy.hpp>

// glyph::children -> glyphs that will be activated by this glyph, in order (aka a timeline)
// glyph::parent -> whatever glyph led to this timeline / thread

// comparing this flow to normal code,
// glyph::parent -> a control flow statement or function 
// glyph::children -> if this glyph 

class Glyph : public Thingy {

public:
    Glyph(std::string n);

private:
    
    
};

#endif
