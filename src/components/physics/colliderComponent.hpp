#ifndef COLLIDERCOMPONENT_HPP
#define COLLIDERCOMPONENT_HPP

#include "../component.hpp"
#include "btBulletDynamicsCommon.h"

class Collider : public Component {
CLASS_DECLARATION(Collider)
friend class RigidBody;
protected:
    btCollisionShape* collisionShape = nullptr; //FIX: needs to be deleted in destructor?
public:
    Collider(std::string && initialValue = "Collider");
    Collider(std::string && initialValue, btCollisionShape* shape);
};


#endif
