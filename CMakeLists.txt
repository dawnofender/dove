# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (dream)
set (CMAKE_CXX_STANDARD 23)

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
	  external/glew-1.13.0/include/
	  external/assimp-3.0.1270/include/
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

add_executable(reality
    src/reality.cpp
    src/TransformVertexShader.vertexshader
    src/TextureFragmentShader.fragmentshader
    src/worldgen/doctree.cpp
    src/worldgen/doctree.hpp

    src/dasset/metadata.cpp
    src/dasset/metadata.hpp
    src/dasset/mesh.cpp
    src/dasset/mesh.hpp
    src/dasset/texture.cpp
    src/dasset/texture.hpp
    src/dasset/shader.hpp
    src/dasset/shader.cpp
    src/dasset/material.cpp
    src/dasset/material.hpp

    src/thingy/thing.cpp
    src/thingy/thing.hpp
    src/thingy/thingy.cpp
    src/thingy/thingy.hpp
    src/components/component.cpp
    src/components/component.hpp
    src/components/physicsComponent.cpp
    src/components/physicsComponent.hpp
    src/components/colliderComponent.cpp
    src/components/colliderComponent.hpp
    src/components/rigidBodyComponent.cpp
    src/components/rigidBodyComponent.hpp
    src/components/sphereColliderComponent.cpp
    src/components/sphereColliderComponent.hpp
    src/components/boxColliderComponent.cpp
    src/components/boxColliderComponent.hpp
    src/components/meshRendererComponent.cpp
    src/components/meshRendererComponent.hpp
    src/components/objectRendererComponent.cpp
    src/components/objectRendererComponent.hpp
    src/components/skyRendererComponent.cpp
    src/components/skyRendererComponent.hpp
    src/components/transformComponent.cpp
    src/components/transformComponent.hpp
    src/components/playerControllerComponent.cpp
    src/components/playerControllerComponent.hpp
    # src/components/gaiaComponent.cpp
    # src/components/gaiaComponent.hpp

    lib/shaderLoader.cpp
    lib/shaderLoader.hpp
    lib/SimplexNoise.cpp
    lib/SimplexNoise.h
    lib/FastNoise.cpp
    lib/FastNoise.h
    lib/controls.cpp
    lib/controls.hpp
    lib/texture.cpp
    lib/texture.hpp
    lib/threadsafe_vector.hpp
    lib/bulletDebugDrawer.cpp
    lib/bulletDebugDrawer.hpp

    lib/stb_image.h
)

target_link_libraries(reality ${ALL_LIBS})
