#include "thingy.hpp"
#include <algorithm>
#include <vector>

CLASS_DEFINITION(Thing, Thingy)



void Thingy::serialize(Archive& archive) {
    archive & name & components & children;
}

void Thingy::init() {
    std::cout << "initializing " << name << std::endl;
    for (auto && child : children) {
        if (!weak_from_this().expired()) {
            child->parent = new_shared_from_this();
        } else {
            std::cerr << "ERROR: Thingy " << name << ": init: thingy does not have a shared_ptr to it, cannot set child's parent" << std::endl;
        }
    }
}

Thingy::~Thingy() {
    std::cout << "deleting " << name << std::endl;

    for ( auto&& child : children ) {
        //move children 
        if (auto p = parent.lock()) {
            p->addChild(child);
        } else {
            std::cerr << name << ": Destructor error: Failed to lock parent ptr" << std::endl;
            child.reset();
            // handle whatever happens if locking the parent ptr doesnt work
        }
    }
    
    for ( auto &&component : components ) {
        component.reset();
    }
}

bool Thingy::addComponent(std::string && typeName) {
    std::unique_ptr<Thing> base = ThingFactory::instance().create(typeName); 

    if (!base) {
        std::cerr << "ERROR: " << name << ": addComponent (" << typeName << "): ThingFactory instantiation failed" << std::endl;
        return false;
    }

    if (Component* cast = dynamic_cast<Component*>(base.get())) {
        std::unique_ptr<Component> component(cast);
        components.emplace_back(std::move(component));
        cast->initOnce();
        base.release();
        return true;
    }

    return false;
}

std::shared_ptr<Thingy> Thingy::getParent() {
    if (auto p = parent.lock())
        return p;
    return nullptr;
}

Thingy& Thingy::addChild(std::string n) {
    auto child = std::make_shared<Thingy>(n);
    addChild(child);
    return *child;
}


void Thingy::addChild(std::shared_ptr<Thingy> child) {
    if (!weak_from_this().expired() && child == new_shared_from_this()) {
        std::cerr << "ERROR: Thingy \"" << name << ": " << "cannot add itself as a child" << std::endl;
        return;
    }

    children.push_back(child);
    
    // if our thing doesn't have a shared ptr to it, shared_from_this will fail
    if (!weak_from_this().expired()) {
        child->parent = new_shared_from_this();
    } else {
        std::cerr << name << ": addchild error: thingy does not have a shared_ptr to it, cannot set child's parent" << std::endl;
    }
}


// remove child by shared_ptr reference
bool Thingy::removeChild(std::shared_ptr<Thingy> &child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        children.erase(it);
        return true;
    }
    return false;
}


bool Thingy::removeChild(Thingy *&thingy) {
    auto it = std::find_if(children.begin(), children.end(),
        [thingy](const std::shared_ptr<Thingy>& child) {
            return child.get() == thingy;
        });
    if (it != children.end()) {
        children.erase(it);
        return true;
    }
    return false;
}


// remove child by name
bool Thingy::removeChild(std::string childName) {
    auto it = std::find_if(children.begin(), children.end(),
        [&childName](const std::shared_ptr<Thingy>& child) {
            return child->getName() == childName;
        });
    if (it != children.end()) {
        children.erase(it);
        return true;
    }
    return false;
}


// remove all children with a given name
int Thingy::removeChildren(std::string childName) {
    int originalSize = children.size();
    children.erase(
        std::remove_if(children.begin(), children.end(),
            [&childName](const std::shared_ptr<Thingy>& child) {
                return child->getName() == childName;
            }),
        children.end());
    return originalSize - children.size();
}


void Thingy::setParent(std::shared_ptr<Thingy> newParent) {
    if (!weak_from_this().expired() && newParent == new_shared_from_this()) {
        std::cerr << "ERROR: Thingy \"" << name << ": " << "cannot set parent to itself" << std::endl;
        return;
    }

    newParent->addChild(new_shared_from_this());
}

void Thingy::setName(std::string && newName) {
    name = std::move(newName);
}

std::string Thingy::getName() {
    return(name);
}

std::shared_ptr<Thingy> Thingy::getRoot() {
    if (auto p = getParent())
        return p->getRoot();
    else 
        return new_shared_from_this();
}
