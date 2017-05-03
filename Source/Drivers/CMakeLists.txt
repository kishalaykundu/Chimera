# Cmake file to make input driven Driver

if (NOT GPU_PACKAGE OR GPU_PACKAGE STREQUAL "OpenGL")
	add_subdirectory (GLDriver)
endif ()
