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

// for components i am currently stealing code from:
// https://stackoverflow.com/questions/44105058/implementing-component-system-from-unity-in-c


// Component base class
class Component : public Thing {
CLASS_DECLARATION(Component)
public: 
    std::string value = "uninitialized";
    // static const std::size_t Type;

    // virtual ~Component() = default;
    
    Component( std::string && initialValue = "")
        : value( initialValue ) { 
    }
    
    virtual void serialize(std::ostream& out) {
        out << value;
    }

    virtual void unserialize(std::istream& in) {
        in >> value;
    }

private: 
  static inline std::vector<Component *> dynamicComponents;

public: 
    static void updateAll(); // calls update() on every dynamic component
    virtual void update();
    void registerDynamic();
    void unregisterDynamic();

// public: 
// virtual weak ptr to host ?
};


#endif
