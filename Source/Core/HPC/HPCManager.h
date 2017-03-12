/**
 * @file HPCManager.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The interface for the high performance computing manager for
 * Chimera. This is in charge of any hardware-accelerated non-
 * graphics related algorithms run by the system. Some examples
 * are CUDA or OpenCL based physics or collision subsystems.
 */
#pragma once

#include "Preprocess.h"

namespace Sim {

	class HPCManager {

		public:
			HPCManager () {}
			virtual ~HPCManager () {}

			virtual bool Initialize (const char* config) = 0;
			virtual void Cleanup () = 0;
	};
}
