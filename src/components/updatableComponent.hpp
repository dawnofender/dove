#ifndef UPDATABLECOMPONENT_HPP
#define UPDATABLECOMPONENT_HPP

#include "component.hpp"
#include <set>

class UpdatableComponent : public Component {
CLASS_DECLARATION(UpdatableComponent)
public:
    UpdatableComponent(std::string && intialName = "UpdatableComponent");
    virtual ~UpdatableComponent();
    virtual void update();
    static void updateAll();
private:
    static inline std::set<UpdatableComponent *> updatableComponents;
};


#endif
