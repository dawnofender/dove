#ifndef EVENTTHING_HPP
#define EVENTTHING_HPP

#include "../../core/thingy/thingy.hpp"

// rename to "moment" or something more action / change related? 
// "event" may be confusing as we also will have an event loop each frame which has little to do with this
// but the event loop could also just be called something else, 
// i just think people are used to talking about event order using the words event order
// which has nothing to do with this


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
