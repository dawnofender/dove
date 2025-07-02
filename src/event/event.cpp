#include "event.hpp"

CLASS_DEFINITION(Thing, Event)


Event::Event(std::shared_ptr<Thingy> w) 
    : world(w) {}

Event::~Event() {
    next.reset();
    // may have to run restore here,
    // but that could be dangerous if the event timeline is accidentally deleted. it would undo everything, with no way to go back.
    // 
}


void Event::restore() {
    if (next)
        next->restore();
}

void Event::invoke() {
    next.reset();
}
