#include "dthing.hpp"
#include <vector>


template<typename CompType, typename... Args>
CompType& Thing::addComponent(Args&&... args) {
    auto component = std::make_shared<CompType>(std::forward<Args>(args)...);
    return components[typeid(CompType)];
}

template<typename CompType>
CompType* Thing::getComponent() {
    auto i = components.find(std::type_index(typeid(CompType)));
    return i == components.end() ? nullptr : static_cast<CompType*>(i->second);
}

Thing& Thing::createChild() {
    auto child = new Thing;
    children.push_back(std::make_shared<Thing>(child));
    return(*child);
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
