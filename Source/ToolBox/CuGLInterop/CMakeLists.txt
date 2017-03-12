# Cmake file for the IdGenerator
project (CUGL CXX)

# Set include directories
include_directories (./ ${SIM_SOURCE_DIR}/Common)

# Set required libraries - OpenGL related
set (CUGL_REQUIRED_LIBS ${CUGL_REQUIRED_LIBS} ${GLX_LIB} ${X11_LIB} ${GL_LIB})

# Set required libraries - thread related
set (CUGL_REQUIRED_LIBS ${CUGL_REQUIRED_LIBS} ${THREAD_LIB})

# Set required libraries - CUDA related
set (CUGL_REQUIRED_LIBS ${CUGL_REQUIRED_LIBS} ${CUDA_PACKAGE})

# Set source files
set (CUGL_SRCS
	./Mesh.cpp
	./GLDisplay.cpp
	./CuPhyiscs.cpp
	./main.cpp)

# Set and link target
add_executable (cuglTest ${CUGL_SRCS})
target_link_libraries (cuglTest ${CUGL_REQUIRED_LIBS})
install (TARGETS cuglTest DESTINATION Bin)

# Set compiler flags in addition to the globally set ones
set (CUGL_COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
set_target_properties (cuglTest PROPERTIES COMPILE_FLAGS ${CUGL_COMPILE_FLAGS})