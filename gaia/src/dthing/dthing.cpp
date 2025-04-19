#include "dthing.hpp"
#include <vector>


Thing& Thing::createChild(std::string n) {
    auto child = std::make_shared<Thing>(n);
    // if our thing doesn't have a shared ptr to it, shared_from_this will fail
    if (!weak_from_this().expired()) {
        child->parent = shared_from_this();
    } else {
        //handle it differently idk
    }
    children.push_back(child);
    return *child;
}

void Thing::setParent(std::shared_ptr<Thing> p) {
    parent = p;
}

void Thing::setName(std::string n) {
    name = n;
}

std::string Thing::getName() {
    return(name);
}
