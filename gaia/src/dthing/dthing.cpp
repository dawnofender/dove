#include "dthing.hpp"
#include <vector>

template< class ComponentType, typename... Args >
void Thing::addComponent(Args&&... args) {
    components.insert( std::make_unique<ComponentType>( std::forward<Args>(args)... ) );
    //auto component = std::make_shared<ComponentType>(std::forward<Args>(args)...);
}

template<typename ComponentType>
ComponentType& Thing::getComponent() {
    auto i = components.find(std::type_index(typeid(ComponentType)));
    return i == components.end() ? nullptr : static_cast<ComponentType*>(i->second);
}

Thing& Thing::createChild() {
    children.push_back(std::make_shared<Thing>());
    return(*children.back());
}

void Thing::setParent(std::shared_ptr<Thing>(p)) {
    parent = p;
}

void Thing::setName(char* n) {
    name = n;
}

char* Thing::getName() {
    return(name);
}

std::vector<std::shared_ptr<Thing>>& Thing::getChildren() {
    return(children);
}
