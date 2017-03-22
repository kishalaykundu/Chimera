# Cmake file for the OpenGL-CUDA-IntelTBB Driver/Application layer
project (APP CXX)

# Set include directory paths
include_directories (
	${GPU_INCLUDE_PATH}
	${COMPUTE_INCLUDE_PATH}
	${SCHEDULER_INCLUDE_PATH}
	${SIM_SOURCE_DIR}/Packages/FastCallback
	${SIM_SOURCE_DIR}/Packages/TBB/include
	${SIM_SOURCE_DIR}/Packages/TinyXML
	${SIM_SOURCE_DIR}/Common
	${SIM_SOURCE_DIR}/Core
	${SIM_SOURCE_DIR}/Drivers)

# Set essential library links
set (APP_REQUIRED_LIBS ${APP_REQUIRED_LIBS} ${DL_LIB} ${MATH_LIB}
		${THREAD_LIB} ${XML_LIB} ${OPENGL_LIBS} ${CUDA_LIBS} ${TBB_LIBS})
	
# Set Core directory path
set (SIM_CORE_DIR ${SIM_SOURCE_DIR}/Core)

# Add source files from other folders
set (APP_SRCS ${APP_SRCS}
	${SIM_SOURCE_DIR}/Common/GL/GLUtils.cpp
	${SIM_SOURCE_DIR}/Common/CUDA/CUDAUtils.cpp
	${SIM_SOURCE_DIR}/Common/Vector.cpp
	${SIM_SOURCE_DIR}/Common/InputParser.cpp
	${SIM_SOURCE_DIR}/Core/Assets/Geometry.cpp)

# Add core source files
file (GLOB ASSETS_DIR_SRCS "${SIM_CORE_DIR}/Assets/*.cpp")
file (GLOB DISPLAY_DIR_SRCS "${SIM_CORE_DIR}/Display/*.cpp")
file (GLOB BASE_DRIVER_DIR_SRCS "${SIM_CORE_DIR}/Driver/*.cpp")
file (GLOB EVENTS_DIR_SRCS "${SIM_CORE_DIR}/Events/*.cpp")
file (GLOB HPC_DIR_SRCS "${SIM_CORE_DIR}/HPC/*.cpp")
file (GLOB PLUGINS_DIR_SRCS "${SIM_CORE_DIR}/Plugins/*.cpp")
file (GLOB TASKS_DIR_SRCS "${SIM_CORE_DIR}/Tasks/*.cpp")

set (APP_SRCS ${APP_SRCS}
	${ASSETS_DIR_SRCS}
	${DISPLAY_DIR_SRCS}
	${BASE_DRIVER_DIR_SRCS}
	${EVENTS_DIR_SRCS}
	${HPC_DIR_SRCS}
	${PLUGINS_DIR_SRCS}
	${TASKS_DIR_SRCS})

# Add platform specific source files
file (GLOB GL45_DIR_SRCS "${SIM_CORE_DIR}/Display/GL45/*.cpp")
file (GLOB CUDA_DIR_SRCS "${SIM_CORE_DIR}/HPC/CUDA/*.cpp")
file (GLOB TBB_DIR_SRCS "${SIM_CORE_DIR}/Tasks/TBB/*.cpp")
file (GLOB DRIVER_DIR_SRCS "${SIM_SOURCE_DIR}/Drivers/GL-CUDA-TBB/*.cpp")

set (APP_SRCS ${APP_SRCS} ${GL45_DIR_SRCS} ${CUDA_DIR_SRCS} ${TBB_DIR_SRCS} ${DRIVER_DIR_SRCS})

# Set and link target
add_executable (simulate ${APP_SRCS})
target_link_libraries (simulate ${APP_REQUIRED_LIBS})
install (TARGETS simulate DESTINATION Bin)

# Set compiler flags in addition to the globally set ones
set (APP_COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -DU_SHOW_CPLUSPLUS_API=0")
set_target_properties (simulate PROPERTIES COMPILE_FLAGS ${APP_COMPILE_FLAGS})
	