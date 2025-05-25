#ifndef DYNAMIC_COMPONENT_HPP
#define DYNAMIC_COMPONENT_HPP

#include <vector>
#include "component.hpp"


class DynamicComponent : public Component {
    CLASS_DECLARATION(Dynamic)
private: 
    static inline std::vector<DynamicComponent *> dynamicComponents;
public: 
    static void updateAll();
    void update();
};


#endif
