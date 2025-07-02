#ifndef DCOMPONENT_HPP
#define DCOMPONENT_HPP

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <cstddef>
#include "../thingy/thing.hpp"
#include "../thingy/archive/archive.hpp"


// Component base class
class Component : public Thing {
CLASS_DECLARATION(Component)
public: 

    Component( std::string && initialValue = "")
        : value( initialValue ) { 
    }

    virtual ~Component() {
        std::cout << "deleting " << value << std::endl;
    }

    virtual void serialize(Archive& ar) override;
    virtual void init() override;

public:
    std::string value = "uninitialized";

// public: 
// weak ptr to host ?
};


#endif
