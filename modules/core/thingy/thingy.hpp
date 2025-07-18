#ifndef DTHINGY_HPP
#define DTHINGY_HPP

#include "../component/component.hpp"
#include <memory>
#include <vector>
// #include <GL/glew.h>
#include <string>
// #include <queue>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "../archive/archive.hpp"


// Thingy-derived classes must be extra special in some way,
// eg. representing some OS object like a file or a window.
// they build on the core features of this class.
// everything else should use components for functionality,

// TODO:
//  - a lot of these functions could be const
//  - derive thingy from an abstract node class (wip)
//  - unique child names? would allow for more efficient getchild, and its better parity with OS filesystem



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
    template<class ComponentType, typename... Args>
    ComponentType& addComponent(Args&&... args);

    // removeComponent removes a component matching the given type and name from this Thingy.
    // Output: true if a component was deleted, false if no matching component was found.
    // Inputs: the name of the component you wish to delete.
    template<class ComponentType>
    bool removeComponent(std::string componentName = "");

    // removeComponents removes all components matching the given type and name from this Thingy.
    // Output: the number of components that have been removed.
    // Inputs: the name of the component(s) you wish to delete.
    template<class ComponentType>
    int removeComponents(std::string componentName = "");

    // getComponent finds a component of the specified type in this thingy's components.
    // Output: a reference to the found component, or nullptr, if no component was found.
    template<class ComponentType>
    ComponentType& getComponent();
    
    // getComponents finds all components of the specified type in this thingy's components.
    // Output: a vector of pointers to each component found, or {nullptr}, if no component was found.
    template<class ComponentType>
    std::vector<ComponentType*> getComponents();

    // addChild creates an object of the specified Thingy-derived type, and adds it as a child to this thingy.
    // Output: a reference to the new child thingy.
    // Inputs: arguments for the new thingy's constructor - varies by type.
    template<class ThingyType, typename... Args>
    ThingyType& addChild(Args&&... args);
    
    // this is just a shorter version for thingies of the base type. it redirects to the function above
    Thingy& addChild(std::string childName = "");

    // addChild(shared_ptr<Thingy>) places the given thingy under this one as a child.
    // Inputs: arguments for the new thingy's constructor - varies by type.
    void addChild(std::shared_ptr<Thingy> thingy);
    
    // getChild finds a child thingy of the given type.
    // Output: a reference to the found thingy.
    template<class ThingyType>
    ThingyType& getChild();
    
    // getChild finds a child thingy of the given type.
    // Output: a reference to the found thingy.
    // Inputs: the name of the child
    Thingy& getChild(std::string childName);
    
    // Traces ancestry back to root, or returns cached root
    Thingy& getRoot();
    

    void setParent(std::shared_ptr<Thingy> thingy);
    void setName(std::string && newName);
    std::string getName();

    // remove 1 child by pointer
    void removeChild(std::shared_ptr<Thingy> &child);
    void removeChild(Thingy *&child);
    
    // remove 1 child by name
    void removeChild(std::string childName);
    
    // remove 1 child of given type
    template<class ThingyType>
    void removeChild();

    // remove 1 child by name and type    
    template<class ThingyType>
    void removeChild(std::string childName);
    
    // remove all children with a given name
    // Output: # of removed children
    int removeChildren(std::string childName);
    
    // remove all children with a given type
    // Output: # of removed children
    template<class ThingyType>
    int removeChildren();

    // remove all children with a given name and type
    // Output: # of removed children
    template<class ThingyType>
    int removeChildren(std::string childName);


    // not implemented yet (2)
    template<class ThingyType>
    std::vector<ThingyType*> getChildren();
    std::vector<std::shared_ptr<Thingy>> getChildren(std::string childName);
    


    void serialize(Archive& ar) override;
    void init() override;
    
};


template< class ComponentType, typename... Args >
ComponentType& Thingy::addComponent(Args&&... args) {
    auto component = std::make_unique<ComponentType>(std::forward<Args>(args)...);
    ComponentType& ref = *component;
    components.emplace_back(std::move(component));
    ref.initOnce();
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
int Thingy::removeComponents(std::string componentName) {
    int removedComponentCount = 0;
    for ( int i = 0; i < components.size(); i++ ) {
        if (components[i]->IsClassType( ComponentType::Type ) && 
            components[i]->name == componentName
        ) {
            components.erase(components.begin() + i);
            removedComponentCount++;
        }
    }
    return removedComponentCount;
}

template< class ComponentType >
bool Thingy::removeComponent(std::string componentName) {
    for ( int i = 0; i < components.size(); i++ ) {
        if (components[i]->IsClassType( ComponentType::Type ) && 
            components[i]->name == componentName
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
    children.emplace_back(child);
    ref.initOnce();
    return ref;
}

// remove 1 child of given type
template<class ThingyType>
void Thingy::removeChild() {
    auto it = std::find_if(children.begin(), children.end(), [](const std::shared_ptr<Thingy>& child) {
        return std::dynamic_pointer_cast<ThingyType>(child) != nullptr;
    });

    if (it != children.end()) {
        children.erase(it);
    }
}

// remove child by name and type
template<class ThingyType>
void Thingy::removeChild(std::string childName) {
    auto it = std::find_if(children.begin(), children.end(), [&childName](const std::shared_ptr<Thingy>& child) {
        return child->getName() == childName && 
            child->IsClassType( ThingyType::Type );
    });
    if (it != children.end()) {
        children.erase(it);
    }
}

// remove all childen with given type
template<class ThingyType>
int Thingy::removeChildren() {
    int originalSize = children.size();
    children.erase(
        std::remove_if(children.begin(), children.end(),
            [](const std::shared_ptr<Thingy>& child) {
                return child->IsClassType( ThingyType::Type );
            }),
        children.end());
    return originalSize - children.size();
}

// remove all children with a given name and type
template<class ThingyType>
int Thingy::removeChildren(std::string childName) {
    int originalSize = children.size();
    children.erase(
        std::remove_if(children.begin(), children.end(),
            [&childName](const std::shared_ptr<Thingy>& child) {
                return child->getName() == childName &&
                       child->IsClassType( ThingyType::Type );
            }),
        children.end());
    return originalSize - children.size();
}


#endif
