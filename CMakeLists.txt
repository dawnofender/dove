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

add_executable(dove
    src/dove.cpp
    src/TransformVertexShader.vertexshader
    src/TextureFragmentShader.fragmentshader
    # src/worldgen/doctree.cpp
    # src/worldgen/doctree.hpp

    src/defaultAssets.hpp
    src/defaultAssets.cpp

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
    src/thingy/glyph/glyphThingy.cpp
    src/thingy/glyph/glyphThingy.hpp
    src/thingy/panel/panelThingy.cpp
    src/thingy/panel/panelThingy.hpp
    src/thingy/panel/windowThingy.cpp
    src/thingy/panel/windowThingy.hpp
    
    src/components/component.cpp
    src/components/component.hpp
    src/components/transformComponent.cpp
    src/components/transformComponent.hpp
    src/components/playerControllerComponent.cpp
    src/components/playerControllerComponent.hpp
    src/components/updatableComponent.cpp
    src/components/updatableComponent.hpp
    # src/components/gaiaComponent.cpp
    # src/components/gaiaComponent.hpp

    src/components/physics/physicsComponent.cpp
    src/components/physics/physicsComponent.hpp
    src/components/physics/colliderComponent.cpp
    src/components/physics/colliderComponent.hpp
    src/components/physics/rigidBodyComponent.cpp
    src/components/physics/rigidBodyComponent.hpp
    src/components/physics/sphereColliderComponent.cpp
    src/components/physics/sphereColliderComponent.hpp
    src/components/physics/boxColliderComponent.cpp
    src/components/physics/boxColliderComponent.hpp

    src/components/rendering/meshRendererComponent.cpp
    src/components/rendering/meshRendererComponent.hpp
    src/components/rendering/objectRendererComponent.cpp
    src/components/rendering/objectRendererComponent.hpp
    src/components/rendering/skyRendererComponent.cpp
    src/components/rendering/skyRendererComponent.hpp
    src/components/rendering/cameraComponent.cpp
    src/components/rendering/cameraComponent.hpp
    
    lib/setup.hpp
    lib/setup.cpp
    lib/shaderLoader.cpp
    lib/shaderLoader.hpp
    lib/stb_image.h
    lib/stb_image.h

    lib/bulletDebugDrawer.cpp
    lib/bulletDebugDrawer.hpp
    
    # used in GAIA
    lib/threadsafe_vector.hpp
    lib/SimplexNoise.cpp
    lib/SimplexNoise.h
    lib/FastNoise.cpp
    lib/FastNoise.h
)

target_link_libraries(dove ${ALL_LIBS})
