#include "thingy.hpp"
#include <vector>


Thingy& Thingy::createChild(std::string n) {
    auto child = std::make_shared<Thingy>(n);
    // if our thing doesn't have a shared ptr to it, shared_from_this will fail
    if (!weak_from_this().expired()) {
        child->parent = shared_from_this();
    } else {
        //handle it differently idk
    }
    children.push_back(child);
    return *child;
}

void Thingy::setParent(std::shared_ptr<Thingy> p) {
    parent = p;
}

void Thingy::setName(std::string n) {
    name = n;
}

std::string Thingy::getName() {
    return(name);
}
