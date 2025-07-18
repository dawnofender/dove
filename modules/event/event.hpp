#ifndef EVENTTHING_HPP
#define EVENTTHING_HPP

#include "../core/thingy/thingy.hpp"


class Event : public Thing {
CLASS_DECLARATION(Event)
public: 
    Event(std::shared_ptr<Thingy> world = nullptr);

    virtual ~Event();
    
    virtual void restore();
    virtual void invoke();

    template<class EventType, typename... Args>
    EventType& addNext(Args&&... args);

protected: 
    std::weak_ptr<Event> last;
    std::shared_ptr<Event> next;

    std::weak_ptr<Thingy> world;
};


template< class EventType, typename... Args >
EventType& Event::addNext(Args&&... args) {
    if (auto w = world.lock()) {
        next = std::make_shared<EventType>(w, std::forward<Args>(args)...);
    } else {
        std::cerr << "ERROR: Event: addNext: failed to lock world weak_ptr" << std::endl;
    }
    EventType& nextEvent = *next;
    nextEvent.init;
    return nextEvent;
}

#endif
