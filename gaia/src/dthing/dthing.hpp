#ifndef DTHING_HPP
#define DTHING_HPP

#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <GL/glew.h>
#include "components/component.hpp"


class Thing {
private: 
    char* name;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
    std::weak_ptr<Thing> parent;
    std::vector<std::shared_ptr<Thing>> children;

public:

    template< class ComponentType, typename... Args >
    void addComponent(Args&&... args);
    //ComponentType& addComponent(Args&&... args);

    template< class ComponentType >
    ComponentType& getComponent();

    // template< class ComponentType >
    // std::vector<ComponentType*> getComponents();

    Thing& createChild();

    std::vector<std::shared_ptr<Thing>>& getChildren();

    void setParent(std::shared_ptr<Thing>(p));

    void setName(char* n);
    char* getName();
    
};


#endif
