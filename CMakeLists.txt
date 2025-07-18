# CMake entry point
cmake_minimum_required (VERSION 3.9)
project (dove VERSION 0.0.1 DESCRIPTION "dreaming")
set (CMAKE_CXX_STANDARD 26)

# symbol visibility
set(CMAKE_CXX_VISIBILITY_PRESET default)
set(CMAKE_VISIBILITY_INLINES_HIDDEN OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


find_package(OpenGL REQUIRED)

# compile external dependencies 
add_subdirectory (external)

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
	# external/AntTweakBar-1.16/include/
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-2.1.0/include/
	external/assimp/include/
	external/bullet3/src/
  	external/SimplexNoise/src/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
    BulletDynamics
    BulletCollision
    LinearMath
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

add_executable(dove
    src/dove.cpp
    
    lib/setup.hpp
    lib/setup.cpp

    # lib/bulletDebugDrawer.cpp
    # lib/bulletDebugDrawer.hpp
    
    # # used in GAIA
    # lib/threadsafe_vector.hpp
    # lib/SimplexNoise.cpp
    # lib/SimplexNoise.h
    # lib/FastNoise.cpp
    # lib/FastNoise.h
)

add_subdirectory (modules)

target_link_libraries(dove ${ALL_LIBS})
