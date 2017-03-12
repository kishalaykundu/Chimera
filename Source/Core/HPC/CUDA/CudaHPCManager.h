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

#include "HPC/HPCManager.h"

namespace Sim {

	class CudaHPCManager : public HPCManager {

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
