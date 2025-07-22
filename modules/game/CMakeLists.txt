
build_module(game
    playerControllerComponent.hpp
    playerControllerComponent.cpp
)

target_link_libraries(game
    PRIVATE transform
    PRIVATE camera
    PRIVATE physics
	PRIVATE ${OPENGL_LIBRARY}
	PRIVATE glfw
	PRIVATE GLEW_1130
    PRIVATE BulletDynamics
    PRIVATE BulletCollision
    PRIVATE LinearMath
)
