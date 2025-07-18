
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
    PRIVATE 3d
    PRIVATE rendering
	PRIVATE ${OPENGL_LIBRARY}
	PRIVATE glfw
	PRIVATE GLEW_1130
    PRIVATE BulletDynamics
    PRIVATE BulletCollision
    PRIVATE LinearMath
)
