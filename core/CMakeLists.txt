


set(DOVE_HEADERS
    rtti_macros.hpp
    thing.hpp
    node.hpp
    thingy/thingy.hpp
    component/component.hpp
    component/updatableComponent.hpp
    archive/archive.hpp

    math.hpp
    math/vector2.hpp
    math/vector3.hpp
    math/vector4.hpp
    math/matrix4x4.hpp
    math/quaternion.hpp
)

set(DOVE_SOURCES
    thing.cpp
    node.cpp
    thingy/thingy.cpp
    component/component.cpp
    component/updatableComponent.cpp
    archive/archive.cpp

    math/vector2.cpp
    math/vector3.cpp
    math/vector4.cpp
    math/matrix4x4.cpp
    math/quaternion.cpp
)

add_library(DoveEngine STATIC
    ${DOVE_SOURCES}
    ${DOVE_HEADERS}
)


