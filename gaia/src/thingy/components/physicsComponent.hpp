#ifndef PHYSICSCOMPONENT_HPP
#define PHYSICSCOMPONENT_HPP

#include "component.hpp"
#include <btBulletDynamicsCommon.h>


class Physics : public Component {
CLASS_DECLARATION(Physics)
public: 
  Physics(std::string &&initialValue);

};



#endif
