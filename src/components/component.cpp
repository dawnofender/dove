#include "component.hpp"

CLASS_DEFINITION(Thing, Component)


void Component::serialize(Archive& ar) {
    ar << value;
}

void Component::deserialize(Archive& ar) {
    ar >> value;
}

// const std::size_t Component::Type = std::hash<std::string>()(TO_STRING(Component));
