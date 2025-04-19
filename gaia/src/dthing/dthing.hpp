#ifndef DTHINGY_HPP
#define DTHINGY_HPP

#include "components/component.hpp"
#include <memory>
#include <vector>
#include <GL/glew.h>
#include <string>


class Thing : public std::enable_shared_from_this<Thing> {
private: 
    std::string name;
    std::shared_ptr<Thing> parent;

public: 
    std::vector<std::unique_ptr<Component>> components;
    std::vector<std::shared_ptr<Thing>> children;

public:
    Thing(std::string n) : name(n) {}
    Thing(const Thing&) = delete;
    Thing& operator=(const Thing&) = delete;

public:
    template<class ComponentType, typename... Args>
    void addComponent(Args&&... args);

    template<class ComponentType>
    ComponentType& getComponent();

    // template< class ComponentType >
    // std::vector<ComponentType*> getComponents();

    Thing& createChild(std::string n);

    void setParent(std::shared_ptr<Thing>(p));

    void setName(std::string n);
    std::string getName();
    
};


template< class ComponentType, typename... Args >
void Thing::addComponent(Args&&... args) {
    components.emplace_back(std::make_unique<ComponentType>(std::forward<Args>(args)...));
}

template< class ComponentType >
ComponentType& Thing::getComponent() {
    for ( auto && component : components ) {
        if ( component->IsClassType( ComponentType::Type ) )
            return *static_cast<ComponentType*>(component.get());
    }
    return *std::unique_ptr<ComponentType>(nullptr);
}


#endif
