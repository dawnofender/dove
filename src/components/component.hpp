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
#include "../thingy/archive/archiveThingy.hpp"



// Component base class
class Component : public Thing {
CLASS_DECLARATION(Component)
public: 

    Component( std::string && initialValue = "")
        : value( initialValue ) { 
    }
    
    virtual void serialize(Archive& ar) override;
    virtual void load() override;

public:
    std::string value = "uninitialized";
    
private: 
  static inline std::vector<Component *> dynamicComponents;

// public: 
// weak ptr to host ?
};


#endif
