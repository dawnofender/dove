#include "command.hpp"

CLASS_DEFINITION(Event, Command)


Command::Command(std::shared_ptr<Thingy> w, std::istream *i) 
    :Event(w), arguments(i) {}


void Command::invoke() {
    Event::invoke();
    execute();
}

void Command::restore() {
    Event::restore();
    undo();
}
