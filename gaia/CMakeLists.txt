# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (dream)
set (CMAKE_CXX_STANDARD 20)

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

    src/dasset/dmesh.cpp
    src/dasset/dmesh.hpp
    src/dasset/dtexture.cpp
    src/dasset/dtexture.hpp
    src/dasset/shader.hpp
    src/dasset/shader.cpp


    src/thingy/thingy.cpp
    src/thingy/thingy.hpp
    src/thingy/components/component.cpp
    src/thingy/components/component.hpp
    src/thingy/components/physicsComponent.cpp
    src/thingy/components/physicsComponent.hpp
    src/thingy/components/colliderComponent.cpp
    src/thingy/components/colliderComponent.hpp
    src/thingy/components/rigidBodyComponent.cpp
    src/thingy/components/rigidBodyComponent.hpp
    src/thingy/components/sphereColliderComponent.cpp
    src/thingy/components/sphereColliderComponent.hpp
    src/thingy/components/meshRendererComponent.cpp
    src/thingy/components/meshRendererComponent.hpp
    # src/thingy/components/spriteRendererComponent.cpp
    # src/thingy/components/spriteRendererComponent.hpp
    src/thingy/components/transformComponent.cpp
    src/thingy/components/transformComponent.hpp
    src/thingy/components/playerControllerComponent.cpp
    src/thingy/components/playerControllerComponent.hpp
    src/thingy/components/gaiaComponent.cpp
    src/thingy/components/gaiaComponent.hpp

    lib/shaderLoader.cpp
    lib/shaderLoader.hpp
    lib/SimplexNoise.cpp
    lib/SimplexNoise.h
    lib/FastNoise.cpp
    lib/FastNoise.h
    # lib/FastNoiseLite.h
    lib/controls.cpp
    lib/controls.hpp
    lib/texture.cpp
    lib/texture.hpp
    lib/threadsafe_vector.hpp

    lib/stb_image.h
)

target_link_libraries(reality ${ALL_LIBS})
