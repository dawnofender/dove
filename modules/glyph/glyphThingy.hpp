#ifndef GLYPHTHINGY_HPP
#define GLYPHTHINGY_HPP

#include "../../core/thing.hpp"



class Glyph : public Thing {
CLASS_DECLARATION(Glyph)
public:
    Glyph(std::string n = "");
    virtual ~Glyph() {};

protected:
    std::string name;
    std::shared_ptr<Glyph> next;
    virtual void execute() {};
};


// arguments
struct Sigil {
    Sigil *next;
    std::weak_ptr<Glyph> glyph;
};


// Glyph that takes arguments
class Spell : public Glyph {
CLASS_DECLARATION(Spell)
public:
    Spell(std::string n = "");
    virtual ~Spell() {};

protected:
    Sigil *arguments;
    
};



#endif
