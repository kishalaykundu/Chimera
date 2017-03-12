# Add all the folders for the toolbox/utilities system

add_subdirectory (IdGenerator)

if (NOT GPU_PACKAGE OR GPU_PACKAGE STREQUAL "OpenGL")
	add_subdirectory (QueryGL)
	
	if (COMPUTE_PACKAGE STREQUAL "CUDA")
		add_subdirectory (CuGLInterop)
	endif ()
endif ()