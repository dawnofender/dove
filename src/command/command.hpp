#ifndef COMMANDEVENT_HPP
#define COMMANDEVENT_HPP

#include "../event/event.hpp"
#include <iostream>

// TODO: 
//  - commands would benefit from a full reflection system like unreal engine's. 
//      - this would also allow for automatic serialization, very convenient

// commands to make:
//  - thingy commands
//      - select <scope[sort, limit]>
//          - no arguments? deselect
//      - create <type> <arguments>
//          - child of selection, no selection assumes root thingy
//          - works just like Thingy::createChild
//      - destroy <type> <arguments>
//          - child of selection, no selection assumes root thingy
//          - works just like Thingy::removeChild
//  - hierarchy commands
//      - cd <path>
//      - mv <name> <destination>
//      - rm <name>

class Command : public Event {
CLASS_DECLARATION(Command)
public: 
    Command(std::shared_ptr<Thingy> world = nullptr, std::istream *arguments = nullptr);

    virtual void restore() override;
    virtual void invoke() override;
    
    virtual void execute() {};
    virtual void undo() {};

protected: 
    std::istream *arguments;

};

#endif
