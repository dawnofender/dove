#ifndef DTHINGY_HPP
#define DTHINGY_HPP

#include "components/component.hpp"
#include <memory>
#include <vector>
#include <GL/glew.h>
#include <string>


class Thingy : public std::enable_shared_from_this<Thingy> {
private: 
    std::string name;
    std::weak_ptr<Thingy> parent;

public: 
    std::vector<std::unique_ptr<Component>> components;
    std::vector<std::shared_ptr<Thingy>> children;

public:
    Thingy(std::string n) : name(n) {}
    virtual ~Thingy();
    Thingy(const Thingy&) = delete;
    Thingy& operator=(const Thingy&) = delete;


public:
    template<class ComponentType, typename... Args>
    void addComponent(Args&&... args);

    template<class ComponentType>
    ComponentType& getComponent();

    // template< class ComponentType >
    // std::vector<ComponentType*> getComponents();

    Thingy& createChild(std::string childName);

    void addChild(std::shared_ptr<Thingy> thingy);
    void removeChild(std::shared_ptr<Thingy> child);
    void removeChild(Thingy* child);
    void setParent(std::shared_ptr<Thingy> thingy);

    void setName(std::string newName);
    std::string getName();
    
};


template< class ComponentType, typename... Args >
void Thingy::addComponent(Args&&... args) {
    components.emplace_back(std::make_unique<ComponentType>(std::forward<Args>(args)...));
}


template< class ComponentType >
ComponentType& Thingy::getComponent() {
    for ( auto && component : components ) {
        if ( component->IsClassType( ComponentType::Type ) )
            return *static_cast<ComponentType*>(component.get());
    }
    return *std::unique_ptr<ComponentType>(nullptr);
}


#endif
