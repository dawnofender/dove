#include "component.hpp"

CLASS_DEFINITION(Thing, Component)
// const std::size_t Component::Type = std::hash<std::string>()(TO_STRING(Component));

void Component::update() {}

void Component::updateAll() {
  for (auto &component : dynamicComponents) {
    component->update();
  }
}

void Component::registerDynamic() {
    dynamicComponents.push_back(this);
}

void Component::unregisterDynamic() {
    dynamicComponents.erase(std::remove(dynamicComponents.begin(), dynamicComponents.end(), this), dynamicComponents.end());
}
