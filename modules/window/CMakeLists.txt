
build_module(window
    window.cpp
    window.hpp
)

target_link_libraries(window
	  PRIVATE panel
	  PRIVATE rendering
	  PRIVATE ${OPENGL_LIBRARY}
	  PRIVATE glfw
	  PRIVATE GLEW_1130
    # PRIVATE BulletDynamics
    # PRIVATE BulletCollision
    # PRIVATE LinearMath
)
