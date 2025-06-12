#ifndef PHYSICSCOMPONENT_HPP
#define PHYSICSCOMPONENT_HPP

#include "../component.hpp"
#include "../../thingy/thingy.hpp"
#include <vector>
#include <glm/glm.hpp>
#include "../rendering/cameraComponent.hpp"
#include <btBulletDynamicsCommon.h>
#include <lib/bulletDebugDrawer.hpp>

struct RayCastInfo {
    bool hasHit;
    Thingy* thingy;
    glm::vec3 position;
    glm::vec3 normal;
};

class Physics : public Component {
CLASS_DECLARATION(Physics)
public: 
    btDiscreteDynamicsWorld* dynamicsWorld;

public:
    Physics(std::string &&initialValue = "Physics");
    ~Physics();

    RayCastInfo rayCast(glm::vec3 position, glm::vec3 direction, float distance);
    void debugDrawWorld();
    static void debugDrawAll();

private:
    static inline std::vector<btDiscreteDynamicsWorld *> dynamicsWorlds;
    // btOverlapFilterCallback* filterCallback;
	// btOverlappingPairCache* pairCache;
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;

    static inline BulletDebugDrawer_DeprecatedOpenGL mydebugdrawer;     

};


#endif
