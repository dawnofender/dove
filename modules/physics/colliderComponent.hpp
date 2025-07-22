#ifndef COLLIDERCOMPONENT_HPP
#define COLLIDERCOMPONENT_HPP

#include "../../core/component/component.hpp"
#include "btBulletDynamicsCommon.h"

class Collider : public Component {
CLASS_DECLARATION(Collider)
friend class RigidBody;
protected:
    btCollisionShape* collisionShape = nullptr; //FIX: needs to be deleted in destructor?
public:
    Collider(std::string && initialName = "Collider");
    Collider(std::string && initialName, btCollisionShape* shape);
};


#endif
