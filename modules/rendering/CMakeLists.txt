
build_module(rendering
    opengl.cpp
    meshRendererComponent.cpp
    meshRendererComponent.hpp
    modelRendererComponent.cpp
    modelRendererComponent.hpp
    skyRendererComponent.cpp
    skyRendererComponent.hpp
)

target_link_libraries(rendering
    PRIVATE 3d
    PRIVATE asset
	PRIVATE ${OPENGL_LIBRARY}
	PRIVATE glfw
	PRIVATE GLEW_1130
    PRIVATE BulletDynamics
    PRIVATE BulletCollision
    PRIVATE LinearMath
)
