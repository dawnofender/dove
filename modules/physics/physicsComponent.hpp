#ifndef PHYSICSCOMPONENT_HPP
#define PHYSICSCOMPONENT_HPP

#include <core/component/component.hpp>
#include <core/thingy/thingy.hpp>
#include <unordered_map>
#include <vector>
#include "../camera/cameraComponent.hpp"
#include <btBulletDynamicsCommon.h>
#include <lib/bulletDebugDrawer.hpp>
#include <set>
#include <core/math.hpp>

// TODO: 
//  - closest surface / spherecast function
//  - 
struct RayCastInfo {
    bool hasHit;
    Thingy* thingy;
    Dove::Vector3 position;
    Dove::Vector3 normal;
};

struct CollisionInfo {
    const Thingy* thingyA;
    const Thingy* thingyB;
    const Dove::Vector3 pointA;
    const Dove::Vector3 pointB;
    const Dove::Vector3 normalOnB;
};

class Physics : public Component {
CLASS_DECLARATION(Physics)
public: 
    
    // TODO: 
    //  - move time get into our own class
    //  - deltatime shouldn't be framerate linked by default, as this causes various potential issues:
    //    - unstable framerate -> unstable physics 
    //    - makes implementing multithreading difficult
    //  make this optional
    //    - this could just be a bool + if statement, but ... timeline polymorphism
    //  - implement sphereCast 
    
    btDiscreteDynamicsWorld* dynamicsWorld;
    double deltaTime;
    double lastTime;
    double time;
    std::vector<std::shared_ptr<CollisionInfo>> collisions;
    std::unordered_map<Thingy*, std::vector<std::shared_ptr<CollisionInfo>>> collisionMap;

public:
    Physics(std::string &&initialName = "Physics");
    ~Physics();
    virtual void init() override;

    RayCastInfo rayCast(Dove::Vector3 position, Dove::Vector3 direction, float distance);
    RayCastInfo sphereCast(Dove::Vector3 position, float distance); // not implemented yet
    std::vector<std::shared_ptr<CollisionInfo>> getCollisionInfo(Thingy* thingy);

    void debugDrawWorld();
    static void debugDrawAll();
    static void simulateAll();
    void simulate();

private:
    static inline std::vector<btDiscreteDynamicsWorld *> dynamicsWorlds;
    static inline std::unordered_map<btDiscreteDynamicsWorld*, Physics*> worldMap;
    static void OnSimulationTick(btDynamicsWorld *world, btScalar timeStep);
    
    // btOverlapFilterCallback* filterCallback;
	// btOverlappingPairCache* pairCache;
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;

    static inline BulletDebugDrawer_DeprecatedOpenGL mydebugdrawer;     
    static inline std::set<Physics*> physicsWorlds;
};


#endif
