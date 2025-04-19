#include "component.hpp"

const std::size_t Component::Type = std::hash<std::string>()(TO_STRING(Component));
