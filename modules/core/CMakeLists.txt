
add_library(common SHARED
    rtti_macros.hpp
    thing.cpp
    thing.hpp
    thingy/thingy.cpp
    thingy/thingy.hpp
    component/component.cpp
    component/component.hpp
    component/updatableComponent.cpp
    component/updatableComponent.hpp
    archive/archive.cpp
    archive/archive.hpp
)

set_target_properties(common PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules
)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
