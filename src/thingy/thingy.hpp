#ifndef DTHINGY_HPP
#define DTHINGY_HPP

#include "../components/component.hpp"
#include <memory>
#include <vector>
#include <GL/glew.h>
#include <string>
#include <queue>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <stack>
#include <iomanip>
#include <sstream>

class Archive;

class Thingy : public Thing {
CLASS_DECLARATION(Thingy)
protected:
    std::string name;
    std::weak_ptr<Thingy> parent;
public: 
    std::vector<std::unique_ptr<Component>> components;
    std::vector<std::shared_ptr<Thingy>> children;

public:
    Thingy(std::string n = "") : name(n) {}
    virtual ~Thingy();
    // Thingy(const Thingy&) = delete;
    // Thingy& operator=(const Thingy&) = delete;

public:
    
    // addComponent creates a component, of any specified type, attached to this Thingy.
    // Output: returns a reference to the new component.
    // Inputs: arguments for the new component's constructor - varies by type.
    // Example: thingy.addComponent<Transform>("Transform") creates a component of type Transform and returns a reference to it.
    template<class ComponentType, typename... Args>
    ComponentType& addComponent(Args&&... args);

    // removeComponent removes a component matching the given type and value from this Thingy.
    // Output: true if a component was deleted, false if no matching component was found.
    // Inputs: the value, or name, of the component you wish to delete.
    // Example: thingy.removeComponent<Transform>("Transform") may remove a single Transform component and return true.
    template<class ComponentType>
    bool removeComponent(std::string value = "");

    // removeComponents removes all components matching the given type and value from this Thingy.
    // Output: the number of components that have been removed.
    // Inputs: the value, or name, of the component(s) you wish to delete.
    // Example: thingy.removeComponents<Transform>("Transform") may remove a single Transform component and return 1.
    template<class ComponentType>
    int removeComponents(std::string value = "");

    // getComponent finds a component of the specified type in this thingy's components.
    // Output: a reference to the found component, or nullptr, if no component was found.
    // Inputs: N/A
    // Example: thingy.getComponent<Transform>() may return a reference to a Transform component.
    template<class ComponentType>
    ComponentType& getComponent();
    
    // getComponents finds all components of the specified type in this thingy's components.
    // Output: a vector of pointers to each component found, or {nullptr}, if no component was found.
    // Inputs: N/A
    // Example: thingy.getComponents<Transform>() may return a vector containing one pointer to a Transform component.
    template<class ComponentType>
    std::vector<ComponentType*> getComponents();

    // addChild creates an object of the specified Thingy-derived type, and adds it as a child to this thingy.
    // Output: a reference to the new child thingy.
    // Inputs: arguments for the new thingy's constructor - varies by type.
    // Example: thingy.addChild<Window>("window") creates a child thingy of type Window and returns a reference to it.
    template<class ThingyType, typename... Args>
    ThingyType& addChild(Args&&... args);
    
    // this is just a shorter version for thingies of the base type. it redirects to the function above
    Thingy& addChild(std::string childName = "");

    // addChild(shared_ptr<Thingy>) places the given thingy under this one as a child.
    // Output: N/A
    // Inputs: arguments for the new thingy's constructor - varies by type.
    // Example: thingy.addChild<Window>("window") creates a child thingy of type Window and returns a reference to it.
    void addChild(std::shared_ptr<Thingy> thingy);
    
    // getChild finds a child thingy of the given type.
    // Output: a reference to the found thingy.
    // Inputs: N/A
    // Example: thingy.getChild<Window>() may return a reference to a child of type Window.
    template<class ThingyType>
    ThingyType& getChild();
    
    // getChild finds a child thingy of the given type.
    // Output: a reference to the found thingy.
    // Inputs: the name of the child
    // Example: thingy.getChild(window) may return a reference to a child named "window".
    // FIX: this one probably doesn't work on derived types
    Thingy& getChild(std::string childName);
    

    void setParent(std::shared_ptr<Thingy> thingy);
    void setName(std::string newName);
    std::string getName();

    // not implemented yet (2)
    template<class ThingyType>
    std::vector<ThingyType*> getChildren();
    std::vector<std::shared_ptr<Thingy>> getChildren(std::string childName);

    void removeChild(std::shared_ptr<Thingy> child);
    void removeChild(Thingy* child);

    // not implemented yet (2)
    void removeChild(std::string childName);
    void removeChildren(std::string childName);

    void serialize(Archive& ar) override;
    void load() override;
};


template< class ComponentType, typename... Args >
ComponentType& Thingy::addComponent(Args&&... args) {
    auto component = std::make_unique<ComponentType>(std::forward<Args>(args)...);
    ComponentType& ref = *component;
    components.emplace_back(std::move(component));
    components.back()->load();
    return ref;
}


template< class ComponentType >
ComponentType& Thingy::getComponent() {
    for ( auto && component : components ) {
        if ( component->IsClassType( ComponentType::Type ) )
            return *static_cast<ComponentType*>(component.get());
    }
    return *std::unique_ptr<ComponentType>(nullptr);
}


template< class ComponentType >
int Thingy::removeComponents(std::string value) {
    int removedComponentCount = 0;
    for ( int i = 0; i < components.size(); i++ ) {
        if (components[i]->IsClassType( ComponentType::Type ) && 
            components[i]->value == value
        ) {
            components.erase(components.begin() + i);
            removedComponentCount++;
        }
    }
    return removedComponentCount;
}

template< class ComponentType >
bool Thingy::removeComponent(std::string value) {
    for ( int i = 0; i < components.size(); i++ ) {
        if (components[i]->IsClassType( ComponentType::Type ) && 
            components[i]->value == value
        ) {
            components.erase(components.begin() + i);
            return true;
        }
    }
    return false;
}

template< class ComponentType >
std::vector<ComponentType*> Thingy::getComponents() {
    std::vector<ComponentType*> componentsOfType;
    for ( auto && component : components ) {
        if ( component->IsClassType( ComponentType::Type ) )
            componentsOfType.emplace_back( static_cast<ComponentType*>( component.get() ) );
    }
    return componentsOfType;
}

template< class ThingyType, typename... Args >
ThingyType& Thingy::addChild(Args&&... args) {
    auto child = std::make_shared<ThingyType>(std::forward<Args>(args)...);
    ThingyType& ref = *child;
    components.emplace_back(child);
    return ref;
}


#endif
