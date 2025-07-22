#ifndef PANELTHINGY_HPP
#define PANELTHINGY_HPP

#include "../../core/thingy/thingy.hpp"


class Panel : public Thingy {
CLASS_DECLARATION(Panel)
public:
    Panel(std::string && initialName = "Panel");

};


#endif
