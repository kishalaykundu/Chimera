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
#include "HPC/CUDA/CudaHPCManager.h"

namespace Sim {

	CudaHPCManager::CudaHPCManager ()
	{
		LOG ("CUDA HPC manager constructed");
	}

	CudaHPCManager::~CudaHPCManager ()
	{
		Cleanup ();
		LOG ("CUDA HPC manager destroyed");
	}

	CudaHPCManager::CudaHPCManager (const CudaHPCManager& hm) {}

	CudaHPCManager& CudaHPCManager::operator = (const CudaHPCManager& hm) {return *this;}

	bool CudaHPCManager::Initialize (const char* configfile)
	{
		return true;
	}

	void CudaHPCManager::Cleanup ()
	{

	}
}
