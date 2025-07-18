# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (dream)
set (CMAKE_CXX_STANDARD 26)

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
    src/TransformVertexShader.vertexshader
    src/TextureFragmentShader.fragmentshader

    src/defaultAssets.hpp
    src/defaultAssets.cpp

    modules/asset/asset.cpp
    modules/asset/asset.hpp
    modules/asset/mesh.cpp
    modules/asset/mesh.hpp
    modules/asset/texture.cpp
    modules/asset/texture.hpp
    modules/asset/shader.hpp
    modules/asset/shader.cpp
    modules/asset/material.cpp
    modules/asset/material.hpp

    modules/event/event.cpp
    modules/event/event.hpp
    modules/event/momento.cpp
    modules/event/momento.hpp
    modules/command/command.cpp
    modules/command/command.hpp

    # modules/core/thingy/node.cpp
    # modules/core/thingy/node.hpp

    modules/core/rtti_macros.hpp
    modules/core/thing.cpp
    modules/core/thing.hpp
    modules/core/thingy/thingy.cpp
    modules/core/thingy/thingy.hpp
    modules/core/component/component.cpp
    modules/core/component/component.hpp
    modules/core/component/updatableComponent.cpp
    modules/core/component/updatableComponent.hpp
    modules/core/archive/archive.cpp
    modules/core/archive/archive.hpp
    
    modules/dove/transformComponent.cpp
    modules/dove/transformComponent.hpp
    modules/dove/playerControllerComponent.cpp
    modules/dove/playerControllerComponent.hpp
    # modules/gaia/gaiaComponent.cpp
    # modules/gaia/gaiaComponent.hpp

    modules/glyph/glyphThingy.cpp
    modules/glyph/glyphThingy.hpp
    modules/panel/panelThingy.cpp
    modules/panel/panelThingy.hpp
    modules/window/window.cpp
    modules/window/window.hpp
    
    modules/physics/physicsComponent.cpp
    modules/physics/physicsComponent.hpp
    modules/physics/colliderComponent.cpp
    modules/physics/colliderComponent.hpp
    modules/physics/rigidBodyComponent.cpp
    modules/physics/rigidBodyComponent.hpp
    modules/physics/sphereColliderComponent.cpp
    modules/physics/sphereColliderComponent.hpp
    modules/physics/boxColliderComponent.cpp
    modules/physics/boxColliderComponent.hpp

    modules/rendering/meshRendererComponent.cpp
    modules/rendering/meshRendererComponent.hpp
    modules/rendering/modelRendererComponent.cpp
    modules/rendering/modelRendererComponent.hpp
    modules/rendering/skyRendererComponent.cpp
    modules/rendering/skyRendererComponent.hpp
    modules/rendering/cameraComponent.cpp
    modules/rendering/cameraComponent.hpp
    
    lib/setup.hpp
    lib/setup.cpp
    lib/shaderLoader.cpp
    lib/shaderLoader.hpp
    lib/assetImporter.hpp
    lib/assetImporter.cpp
    lib/stb_image.h
    lib/stb_image.h

    lib/bulletDebugDrawer.cpp
    lib/bulletDebugDrawer.hpp
    
    # # used in GAIA
    # lib/threadsafe_vector.hpp
    # lib/SimplexNoise.cpp
    # lib/SimplexNoise.h
    # lib/FastNoise.cpp
    # lib/FastNoise.h
)

target_link_libraries(dove ${ALL_LIBS})
