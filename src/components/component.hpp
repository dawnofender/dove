#ifndef DCOMPONENT_HPP
#define DCOMPONENT_HPP

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <cstddef>
#include <src/thingy/thing.hpp>



// Component base class
class Component : public Thing {
CLASS_DECLARATION(Component)
public: 

    Component( std::string && initialValue = "")
        : value( initialValue ) { 
    }
    
    virtual void serialize(std::ostream& out) {
        out << value;
    }

    virtual void unserialize(std::istream& in) {
        in >> value;
    }

    std::string value = "uninitialized";
    
private: 
  static inline std::vector<Component *> dynamicComponents;

// public: 
// weak ptr to host ?
};


#endif
