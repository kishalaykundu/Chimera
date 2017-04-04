# Cmake file for the IdGenerator
project (GTA CXX)

# Set include directories
include_directories (${GPU_INCLUDE_PATH}
	${SIM_SOURCE_DIR}/Packages/TinyXML
	${SIM_SOURCE_DIR}/Packages/LodePNG
	${SIM_SOURCE_DIR}/Packages/Eigen3
	${SIM_SOURCE_DIR}/Common)

# Set required libraries - OpenGL related
set (GTA_REQUIRED_LIBS ${OPENGL_LIBS} ${DL_LIB} ${XML_LIB} ${PNG_LIB})

# Set source files
set (GTA_SRCS
	${SIM_SOURCE_DIR}/Common/Crc32.cpp
	${SIM_SOURCE_DIR}/Common/Vector.cpp
	${SIM_SOURCE_DIR}/Common/Matrix.cpp
	${SIM_SOURCE_DIR}/Common/InputParser.cpp
	${SIM_SOURCE_DIR}/Common/GL/GLUtils.cpp
	./GLMesh.cpp
	./main.cpp)

# Set and link target
add_executable (glGenAtlas ${GTA_SRCS})
target_link_libraries (glGenAtlas ${GTA_REQUIRED_LIBS})
install (TARGETS glGenAtlas DESTINATION Bin)

# Set compiler flags in addition to the globally set ones
set (GTA_COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
set_target_properties (glGenAtlas PROPERTIES COMPILE_FLAGS ${GTA_COMPILE_FLAGS})
