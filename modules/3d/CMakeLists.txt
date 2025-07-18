
build_module(3d
    transformComponent.hpp
    transformComponent.cpp
)

target_link_libraries(3d
	# PRIVATE ${OPENGL_LIBRARY}
	# PRIVATE glfw
	PRIVATE GLEW_1130
    # PRIVATE BulletDynamics
    # PRIVATE BulletCollision
    # PRIVATE LinearMath
)
