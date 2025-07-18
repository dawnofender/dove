build_module(asset
    asset.cpp
    asset.hpp
    mesh.cpp
    mesh.hpp
    texture.cpp
    texture.hpp
    stb_image.h
    shader.hpp
    shader.cpp
    material.hpp
    material.cpp
)

target_link_libraries(asset
	PRIVATE ${OPENGL_LIBRARY}
	PRIVATE glfw
	PRIVATE GLEW_1130
)
