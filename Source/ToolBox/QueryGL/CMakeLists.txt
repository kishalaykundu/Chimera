# Cmake file for the IdGenerator
project (QUERYGL CXX)

# Set include directories
include_directories (${GPU_INCLUDE_PATH} ${SIM_SOURCE_DIR}/Common)

# Set required libraries - OpenGL related
set (QGL_REQUIRED_LIBS ${QGL_REQUIRED_LIBS} ${GLX_LIB} ${X11_LIB} ${GL_LIB})

# Set source files
set (QGL_SRCS ./main.cpp)

# Set and link target
add_executable (queryGL ${QGL_SRCS})
target_link_libraries (queryGL ${QGL_REQUIRED_LIBS})
install (TARGETS queryGL DESTINATION Bin)

# Set compiler flags in addition to the globally set ones
set (QGL_COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
set_target_properties (queryGL PROPERTIES COMPILE_FLAGS ${QGL_COMPILE_FLAGS})