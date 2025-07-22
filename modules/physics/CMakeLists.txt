
build_module(physics
    physicsComponent.cpp
    physicsComponent.hpp
    colliderComponent.cpp
    colliderComponent.hpp
    rigidBodyComponent.cpp
    rigidBodyComponent.hpp
    sphereColliderComponent.cpp
    sphereColliderComponent.hpp
    boxColliderComponent.cpp
    boxColliderComponent.hpp
)

target_link_libraries(physics
    PRIVATE transform
	PRIVATE GLEW_1130
    PRIVATE BulletDynamics
    PRIVATE BulletCollision
    PRIVATE LinearMath
)
