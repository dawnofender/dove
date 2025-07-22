#include "thingy.hpp"
#include <algorithm>
#include <vector>


Thingy::~Thingy() {

    for ( auto&& child : children ) {
        //move children 
        if (auto p = parent.lock()) {
            p->addChild(child);
            child.reset();
        } else {
            // handle whatever happens if locking the parent ptr doesnt work
        }
    }

}

Thingy& Thingy::createChild(std::string n) {
    auto child = std::make_shared<Thingy>(n);
    addChild(child);
    return *child;
}

void Thingy::addChild(std::shared_ptr<Thingy> child) {
    children.push_back(child);
    
    // if our thing doesn't have a shared ptr to it, shared_from_this will fail
    if (!weak_from_this().expired()) {
        child->parent = shared_from_this();
    } else {
        //handle it differently idk
    }
}
        
void Thingy::removeChild(Thingy* thingy) {
    auto it = std::find_if(children.begin(), children.end(),
        [thingy](const std::shared_ptr<Thingy>& child) {
            return child.get() == thingy;
        });
    
    if (it != children.end()) {
        children.erase(it);  // This will destroy the shared_ptr
    }
}

void Thingy::setParent(std::shared_ptr<Thingy> newParent) {
    newParent->addChild(shared_from_this());
}

void Thingy::setName(std::string n) {
    name = n;
}

std::string Thingy::getName() {
    return(name);
}
