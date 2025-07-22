
build_module(transform
    transformComponent.hpp
    transformComponent.cpp
)

target_link_libraries(transform
	PRIVATE GLEW_1130
)
