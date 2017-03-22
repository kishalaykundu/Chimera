/**
 * @file CudaHPCManager.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The CUDA based High Performance Computing module manager.
 */
#pragma once

#include "Config.h"

#ifdef SIM_GL_ENABLED
#	include "GL/GLUtils.h"
#	include "X11/X11.h"
#endif

#include "CUDA/CUDAUtils.h"
#include "HPC/HPCManager.h"

namespace Sim {

	class CudaHPCManager : public HPCManager {

	protected:
#		ifdef SIM_GL_ENABLED
		::Display* _display;
		::GLXContext _gpuContext;
#		endif

		CUcontext _hpcContext;

	private: // forbidden copy constructor and assignment operator
		CudaHPCManager (const CudaHPCManager&);
		CudaHPCManager& operator = (const CudaHPCManager&);

	public:
		CudaHPCManager ();
		~CudaHPCManager ();

		virtual bool Initialize (const char* config) override;
		virtual void Cleanup () override;
	};
}
