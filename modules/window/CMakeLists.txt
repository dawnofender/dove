
build_module(window
    window.cpp
    window.hpp
)

target_link_libraries(window
	  PRIVATE panel
	  PRIVATE rendering
	  PRIVATE GLEW_1130
)
