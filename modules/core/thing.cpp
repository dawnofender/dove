#include "thing.hpp"

const std::size_t Thing::Type = std::hash<std::string>()(TO_STRING(Thing));
