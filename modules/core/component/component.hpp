#ifndef DCOMPONENT_HPP
#define DCOMPONENT_HPP

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <cstddef>
#include "../thing.hpp"
#include "../archive/archive.hpp"

class Thingy;

// Component base class
class Component : public Thing {
CLASS_DECLARATION(Component)
public: 

    Component( std::string && initialName = "")
        : name( initialName ) { 
    }

    virtual ~Component() {
        std::cout << "deleting " << name << std::endl;
    }

    virtual void serialize(Archive& ar) override;
    virtual void init() override;
    
    // components are kept as light as possible, so we don't have a weak_ptr to the host by default
    // derived components may implement that if they need it
    // useful when copying components 
    virtual void setHost(std::shared_ptr<Thingy> host) {};

public:
    std::string name = "uninitialized";
};


#endif
