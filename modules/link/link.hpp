
#include "../core/thingy/thingy.hpp"

class Link {
public:
    Link(Thingy *player);
    void start();

private:
    Thingy *player;

};
