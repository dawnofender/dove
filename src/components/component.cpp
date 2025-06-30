#include "component.hpp"

CLASS_DEFINITION(Thing, Component)


void Component::serialize(Archive& ar) {
    ar & value;
    std::cout << "archived " << value << std::endl;
}

void Component::init() {
    std::cout << "initializing " << value << std::endl;
}

// const std::size_t Component::Type = std::hash<std::string>()(TO_STRING(Component));
