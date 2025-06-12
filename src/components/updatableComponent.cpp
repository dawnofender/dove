#include "updatableComponent.hpp"

CLASS_DEFINITION(Component, UpdatableComponent)

UpdatableComponent::UpdatableComponent(std::string && initialValue) 
    : Component(std::move(initialValue)) {
    updatableComponents.push_back(this);
}

UpdatableComponent::~UpdatableComponent() {
    updatableComponents.erase(std::remove(updatableComponents.begin(), updatableComponents.end(), this), updatableComponents.end());
}

void UpdatableComponent::updateAll() {
    for (auto&& component : updatableComponents) {
        component->update();
    }
}

void UpdatableComponent::update() {}
