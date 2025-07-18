#include "momento.hpp"

CLASS_DEFINITION(Event, Momento)

Momento::Momento(std::shared_ptr<Thingy> world) 
    : Event(world) {}

void Momento::invoke() {
    Event::invoke();
    
    // save the world's current state into 'data'.
    // archive into an ostringstream:
    std::ostringstream oss;
    Archive archive(&oss);

    if (auto w = world.lock()) {
        archive.serialize(w);
    } else {
        std::cerr << "ERROR: Momento: capture: failed to lock world weak_ptr" << std::endl;
        return;
    }
    
    // copy ostringstream into 'data':
    data = oss.str(); // NOTE: see header for why this may be a problem and a potential solution

}

void Momento::restore() {
    Event::restore();
    
    if (!data.length()) {
        std::cerr << "ERROR: Momento: no data, couldn't restore" << std::endl;
        return;
    }

    // stream archive from data string
    std::istringstream iss(data);
    Archive archive(&iss);


    // commented out is if we decide to use shared ptrs instead
    if (auto w = world.lock()) {
        // may have to reset world to avoid errors, but for now lets just not and see what happens!
        archive.deserialize(w);
        std::cout << "testc" << std::endl;
    } else {
        std::cerr << "ERROR: Momento: restore: failed to lock world weak_ptr" << std::endl;
    }
}
