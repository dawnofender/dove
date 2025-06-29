#include "thingy.hpp"
#include <algorithm>
#include <vector>
#include "archive/archiveThingy.hpp"

CLASS_DEFINITION(Thing, Thingy)

void Thingy::serialize(Archive& ar) {
    ar & name & parent & children & components;
    std::cout << "archiving " << name << ": done" << std::endl;
}

void Thingy::load() {
    std::cout << "loading " << name << std::endl;
}

Thingy::~Thingy() {

    for ( auto&& child : children ) {
        //move children 
        if (auto p = parent.lock()) {
            p->addChild(child);
            child.reset();
        } else {
            std::cerr << name << ": Destructor error: Failed to lock parent ptr, children orphaned" << std::endl;
            // handle whatever happens if locking the parent ptr doesnt work
        }
    }
}


Thingy& Thingy::addChild(std::string n) {
    auto child = std::make_shared<Thingy>(n);
    addChild(child);
    return *child;
}

void Thingy::addChild(std::shared_ptr<Thingy> child) {
    children.push_back(child);
    
    // if our thing doesn't have a shared ptr to it, shared_from_this will fail
    if (!weak_from_this().expired()) {
        child->parent = new_shared_from_this();
    } else {
        std::cerr << name << ": addChild error: Thingy does not have a shared_ptr to it, cannot set child's parent" << std::endl;
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
    newParent->addChild(new_shared_from_this());
}

void Thingy::setName(std::string n) {
    name = n;
}

std::string Thingy::getName() {
    return(name);
}
