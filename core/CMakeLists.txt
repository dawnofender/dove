set(DOVE_HEADERS
    math/mat4x4.hpp
    math/vec3.hpp
    rtti_macros.hpp
    thing.hpp
    node.hpp
    thingy/thingy.hpp
    component/component.hpp
    component/updatableComponent.hpp
    archive/archive.hpp
)

set(DOVE_SOURCES
    math/mat4x4.cpp
    math/vec3.cpp
    thing.cpp
    node.cpp
    thingy/thingy.cpp
    component/component.cpp
    component/updatableComponent.cpp
    archive/archive.cpp
)

add_library(DoveEngine STATIC
    ${DOVE_SOURCES}
    ${DOVE_HEADERS}
)


