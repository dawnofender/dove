#include "component.hpp"

CLASS_DEFINITION(Thing, Component)


void Component::serialize(Archive& ar) {
    ar & name;
    std::cout << "archived " << name << std::endl;
}

void Component::init() {
    std::cout << "initializing " << name << std::endl;
}

// const std::size_t Component::Type = std::hash<std::string>()(TO_STRING(Component));
