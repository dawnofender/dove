# CMake entry point
cmake_minimum_required (VERSION 3.9)
project (dream VERSION 0.0.1 DESCRIPTION "dreaming")
set (CMAKE_CXX_STANDARD 26)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenGL REQUIRED)

# compile external dependencies 
# TODO: only rendering and physics actually need these, so we should isolate them to those modules
add_subdirectory (external)

include_directories(
	  external/glfw-3.1.2/include/
	  external/glm-0.9.7.1/
	  external/glew-2.1.0/include/
	  external/assimp/include/
	  external/bullet3/src/
 	  external/SimplexNoise/src/
		.
)

# set(ALL_LIBS
# 		${OPENGL_LIBRARY}
# 		glfw
# 		GLEW_1130
#     BulletDynamics
#     BulletCollision
#     LinearMath
# )

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)


################
# compile core #
################
# probably used by all modules
# probably going to rename this dove engine..? but it doesn't have enough content on its own to earn that name
# core + the basic modules makes it an engine
# but maybe i'll move some of the modules back into this later on if they're never separated ever

add_subdirectory(core)
# include_directories(dove)


###################
# compile modules #
###################

# function used in each module's CMakeLists.txt
function(build_module module_name)
    add_library(${module_name} SHARED ${ARGN})
    
    set_target_properties(${module_name} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules
        INSTALL_RPATH "$ORIGIN"
    )
    
    target_include_directories(${module_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${module_name})
		target_link_libraries(${module_name} PRIVATE DoveEngine)
    
    # export symbols for dlsym
    target_compile_definitions(${module_name} PRIVATE -DFOO_EXPORTS)
		
endfunction()

# build all modules in modules/
add_subdirectory (modules)


####################
# compile launcher #
####################
# compile main program

add_executable(dream
    src/dove.cpp
    src/defaultAssets.cpp
    lib/libraryLoader.cpp

    lib/libraryLoader.hpp
    src/defaultAssets.hpp
)

target_link_libraries(dream
    PRIVATE DoveEngine
    PRIVATE transform
    PRIVATE window
    PRIVATE asset
    PRIVATE rendering
    PRIVATE camera
    PRIVATE physics
    PRIVATE game
    PRIVATE link
		PRIVATE GLEW_1130 # temporary for vec3 and mat4 implementation
)

