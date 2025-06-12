#ifndef UPDATABLECOMPONENT_HPP
#define UPDATABLECOMPONENT_HPP

#include "component.hpp"

class UpdatableComponent : public Component {
CLASS_DECLARATION(UpdatableComponent)
public:
    UpdatableComponent(std::string && intialValue = "UpdatableComponent");
    ~UpdatableComponent();
    virtual void update();
    static void updateAll();
private:
    static inline std::vector<UpdatableComponent *> updatableComponents;
};


#endif
