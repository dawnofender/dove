
build_module(camera
    cameraComponent.cpp
    cameraComponent.hpp
)

target_link_libraries(camera
    PRIVATE transform
    PRIVATE rendering
    PRIVATE window
	PRIVATE ${OPENGL_LIBRARY}
	PRIVATE glfw
	PRIVATE GLEW_1130
    PRIVATE BulletDynamics
    PRIVATE BulletCollision
    PRIVATE LinearMath
)
