build_module(link
    link.cpp
    link.hpp
)


target_link_libraries(link
    PRIVATE physics
    PRIVATE camera

)
