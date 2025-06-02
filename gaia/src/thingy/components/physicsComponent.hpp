#ifndef PHYSICSCOMPONENT_HPP
#define PHYSICSCOMPONENT_HPP

#include "component.hpp"
#include "../thingy.hpp"
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <glm/glm.hpp>

class Physics : public Component {
CLASS_DECLARATION(Physics)
private:
    // btOverlapFilterCallback* filterCallback;
	// btOverlappingPairCache* pairCache;
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;


public: 
    btDiscreteDynamicsWorld* dynamicsWorld;

public:
    Physics(std::string &&initialValue);
    
    Thingy* rayCast(glm::vec3 position, glm::vec3 direction, float distance);
};


// ComponentType& Thingy::addComponent(Args&&... args) {
//     auto component = std::make_unique<ComponentType>(std::forward<Args>(args)...);
//     ComponentType& ref = *component;
//     components.emplace_back(std::move(component));
//     return ref;

#endif
