#ifndef EXPRESSIONCOMPONENT_HPP
#define EXPRESSIONCOMPONENT_HPP

#ifndef CALLBACK
#define CALLBACK
#endif

#include "component.hpp"

// class Expression : public Component {
// CLASS_DECLARATION(Expression)
// public:
//     Expression(std::string && initialValue, )
// };

class InputListener : public Thing {
CLASS_DECLARATION(InputListener)
public:
private: 
    static std::vector<InputListener> inputListeners;
};



#endif
