/**
 * @file CudaHPCManager.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See CudaHPCManager.h.
 */

#include "Preprocess.h"

#include "Driver.h"
#include "HPC/CUDA/CudaHPCManager.h"

namespace Sim {

	CudaHPCManager::CudaHPCManager () :
#	ifdef SIM_GL_ENABLED
		_display (nullptr), _gpuContext (0),
#	endif
		_hpcContext (0)
	{
		LOG ("CUDA HPC manager constructed");
	}

	CudaHPCManager::~CudaHPCManager ()
	{
		Cleanup ();
		LOG ("CUDA HPC manager destroyed");
	}

	CudaHPCManager::CudaHPCManager (const CudaHPCManager& hm) :
#	ifdef SIM_GL_ENABLED
		_display (nullptr), _gpuContext (0),
#	endif
		_hpcContext (0)
	{}

	CudaHPCManager& CudaHPCManager::operator = (const CudaHPCManager& hm) {return *this;}

	bool CudaHPCManager::Initialize (const char* configfile)
	{
#		ifdef SIM_GL_ENABLED

		_display = Driver::Instance().GetGPUDisplay ();
		_gpuContext = glXCreateContextAttribsARB (_display, Driver::Instance().GetGPUConfig (),
				Driver::Instance().GetGPUContext (), true, Driver::Instance().GetGPUContextAttributes ()); LOG_GL_ERROR ();

		if (!_gpuContext){
			LOG_ERROR ("Could not create GLX context for CUDA HPC Manager");
		  return false;
		}
		glXMakeContextCurrent (_display, 0, 0, _gpuContext); LOG_GL_ERROR ();
		XSync (_display, False);

#		endif

		LOG_CUDA_RESULT (cuInit (0));

		// set device and create context (only 1 device associated with current OpenGL context is queried)
		unsigned int numDevices = 0;
		CUdevice device [2];
		LOG_CUDA_RESULT (cuGLGetDevices (&numDevices, device, 1, CU_GL_DEVICE_LIST_ALL));
		LOG_CUDA_ERROR (cudaSetDevice (device [0]));
		LOG_CUDA_RESULT (cuGLCtxCreate (&_hpcContext, CU_CTX_SCHED_AUTO | CU_CTX_MAP_HOST, *device));
		LOG_CUDA_RESULT (cuCtxSetCurrent (_hpcContext));

		LOG ("CUDA HPC manager initialized");
		return true;
	}

	void CudaHPCManager::Cleanup ()
	{

	}
}
