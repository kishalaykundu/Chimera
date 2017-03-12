# Cmake file to make input driven Driver

if ((NOT GPU_PACKAGE OR GPU_PACKAGE STREQUAL "OpenGL") AND
		(COMPUTE_PACKAGE STREQUAL "CUDA") AND
		(SCHEDULER_PACKAGE STREQUAL "IntelTBB"))
	add_subdirectory (GL-CUDA-TBB)
endif ()
