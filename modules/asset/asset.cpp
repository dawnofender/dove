#include "asset.hpp"

CLASS_DEFINITION(Thing, Asset)


Asset::Asset(std::string && initialName)
    : name(std::move(initialName)) {}


void Asset::serialize(Archive& archive) {
    archive & name;
    std::cout << "archived asset: " << name << std::endl;
}

void Asset::init() {
    std::cout << "initializing asset: " << name << std::endl;
}
