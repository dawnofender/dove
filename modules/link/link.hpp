
#include "../../core/thingy/thingy.hpp"


// currently unused, but this will run the main loop that connects the player to the world.

class Link {
public:
    Link(Thingy *player);
    void start();

private:
    Thingy *player;

};
