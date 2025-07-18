
build_module(test
    test.cpp
    defaultAssets.hpp
    defaultAssets.cpp
)

target_link_libraries(test
    PRIVATE 3d
    PRIVATE window
    PRIVATE asset
    PRIVATE rendering
    PRIVATE camera
    PRIVATE physics
    PRIVATE game
    PRIVATE link
)
