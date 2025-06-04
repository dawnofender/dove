#ifndef COLLIDERCOMPONENT_HPP
#define COLLIDERCOMPONENT_HPP

#include "component.hpp"
#include "btBulletDynamicsCommon.h"

class Collider : public Component {
CLASS_DECLARATION(Collider)
friend class RigidBody;
protected:
    btCollisionShape* collisionShape; //FIX: needs to be deleted in destructor?
public:
    Collider( std::string && initialValue, btCollisionShape* shape)
        : Component(std::move( initialValue )), collisionShape(shape) { 
    }
};


#endif
