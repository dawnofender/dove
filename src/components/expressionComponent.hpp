#ifndef EXPRESSIONCOMPONENT_HPP
#define EXPRESSIONCOMPONENT_HPP

#include "component.hpp"

class Expression : public Component {
CLASS_DECLARATION(Expression)
public:
    Expression(std::string && initialValue, )
}

class InputListener : public Thing {
CLASS_DECLARATION(InputListener)
public:
    InputListener();
}

class Input {
public:

protected:
    static std::vector<InputListener> inputListeners;
}


#endif
