#ifndef DTHINGY_HPP
#define DTHINGY_HPP

#include "../components/component.hpp"
#include "../dasset/metadata.hpp"
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
    Thingy(Metadata *m);
    virtual ~Thingy();
    // Thingy(const Thingy&) = delete;
    // Thingy& operator=(const Thingy&) = delete;

public:
    template<class ComponentType, typename... Args>
    ComponentType& addComponent(Args&&... args);

    template<class ComponentType>
    void removeComponent(std::string value = "");

    template<class ComponentType>
    void removeComponents(std::string value = "");

    template<class ComponentType>
    ComponentType& getComponent();
    
    // not implemented yet
    template<class ComponentType>
    std::vector<ComponentType*> getComponents();

    template<class ThingyType, typename... Args>
    ThingyType& addChild(Args&&... args);
    
    void addChild(std::shared_ptr<Thingy> thingy);
    Thingy& addChild(std::string childName = "");
    
    template<class ThingyType>
    ThingyType& getChild();
    
    Thingy& getChild(std::string childName);
    
    // not implemented yet (2)
    template<class ThingyType>
    std::vector<ThingyType*> getChildren();
    std::vector<std::shared_ptr<Thingy>> getChildren(std::string childName);
   

    void removeChild(std::shared_ptr<Thingy> child);
    void removeChild(Thingy* child);

    // not implemented yet (2)
    void removeChild(std::string childName);
    void removeChildren(std::string childName);

    void setParent(std::shared_ptr<Thingy> thingy);
    void setName(std::string newName);
    std::string getName();


    static void serializeHierarchy(Thingy *root, std::string filename);
    
};


template< class ComponentType, typename... Args >
ComponentType& Thingy::addComponent(Args&&... args) {
    auto component = std::make_unique<ComponentType>(std::forward<Args>(args)...);
    ComponentType& ref = *component;
    components.emplace_back(std::move(component));
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
void Thingy::removeComponents(std::string value) {
    for ( int i = 0; i < components.size(); i++ ) {
        if (components[i]->IsClassType( ComponentType::Type ) && 
            components[i]->value == value
        ) {
            components.erase(components.begin() + i);
        }
    }
}

template< class ComponentType >
void Thingy::removeComponent(std::string value) {
    for ( int i = 0; i < components.size(); i++ ) {
        if (components[i]->IsClassType( ComponentType::Type ) && 
            components[i]->value == value
        ) {
            components.erase(components.begin() + i);
            return;
        }
    }
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
