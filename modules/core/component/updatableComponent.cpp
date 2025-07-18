#include "updatableComponent.hpp"

CLASS_DEFINITION(Component, UpdatableComponent)

UpdatableComponent::UpdatableComponent(std::string && initialName) 
    : Component(std::move(initialName)) {
    updatableComponents.insert(this);
}

UpdatableComponent::~UpdatableComponent() {
    updatableComponents.erase(this);
}

void UpdatableComponent::updateAll() {
    for (auto&& component : updatableComponents) {
        component->update();
    }
}

void UpdatableComponent::update() {}
