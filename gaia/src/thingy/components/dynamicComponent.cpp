#include "dynamicComponent.hpp"

CLASS_DEFINITION(Component, DynamicComponent)

void DynamicComponent::updateAll() {
  for (auto &component : dynamicComponents) {
    component->update();
  }
}

void DynamicComponent::update() {}
