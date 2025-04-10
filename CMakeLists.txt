# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (reality)

find_package(OpenGL REQUIRED)

# compile external dependencies 
add_subdirectory (external)

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
	external/AntTweakBar-1.16/include/
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	external/assimp-3.0.1270/include/
	external/bullet-2.81-rev2613/src/
  	external/SimplexNoise/src/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

add_executable(reality
    src/gaia/reality.cpp
    src/gaia/TransformVertexShader.vertexshader
    src/gaia/TextureFragmentShader.fragmentshader
    lib/shader.cpp
    lib/shader.hpp
    lib/SimplexNoise.cpp
    lib/SimplexNoise.h
	  lib/controls.cpp
	  lib/controls.hpp
	  lib/texture.cpp
    lib/texture.hpp
)

target_link_libraries(reality ${ALL_LIBS})
