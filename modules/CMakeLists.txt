
function(build_module module_name)
    add_library(${module_name} SHARED ${ARGN})
    
    set_target_properties(${module_name} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules
        INSTALL_RPATH "$ORIGIN"
    )
    
    target_include_directories(${module_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${module_name})

    target_link_libraries(${module_name} PRIVATE common)
    
    # export symbols for dlsym
    target_compile_definitions(${module_name} PRIVATE -DFOO_EXPORTS)
endfunction()

add_subdirectory(core)
add_subdirectory(3d)
add_subdirectory(asset)
add_subdirectory(panel)
add_subdirectory(window)
add_subdirectory(rendering)
add_subdirectory(camera)
add_subdirectory(physics)
add_subdirectory(game)
add_subdirectory(link)
add_subdirectory(test)
