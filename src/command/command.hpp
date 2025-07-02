#ifndef COMMANDEVENT_HPP
#define COMMANDEVENT_HPP

#include "../event/event.hpp"
#include <iostream>

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
