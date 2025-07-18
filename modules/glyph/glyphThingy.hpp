#ifndef GLYPHTHINGY_HPP
#define GLYPHTHINGY_HPP

#include "../core/thingy/thingy.hpp"

// glyph::children -> glyphs that will be activated by this glyph, in order (aka a timeline)
// glyph::parent -> whatever glyph led to this timeline / thread
// glyph::arguments -> glyphs that need to be accessed by this glyph

// comparing this flow to normal code,
// glyph::parent -> a function or control flow statement 
// glyph::children -> statements inside this statement
// glyph::arguments -> arguments

class Glyph : public Thingy {
CLASS_DECLARATION(Glyph)
public:
    Glyph(std::string n = "");

protected:
    std::vector<std::shared_ptr<Glyph>> arguments;
    
};

#endif
