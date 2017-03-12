/**
 * @file Driver.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The OpenGL/CUDA/TBB driver derived from the BaseDriver class.
 */
#pragma once

#include "Preprocess.h"

#include "Assets/AssetFactory.h"
#include "Display/DisplayManager.h"
#include "Events/EventManager.h"
#include "HPC/HPCManager.h"
#include "Plugins/PluginManager.h"
#include "Tasks/TaskManager.h"
#include "Driver/BaseDriver.h"

namespace Sim {

	class Driver : public BaseDriver {

		protected:
			static Driver* _instance;

		protected:
			Driver () {LOG ("GL-CUDA-TBB Driver constructed");}

		private: // forbidden copy constructor and assignment operator
			Driver (const Driver&) {}
			Driver& operator = (const Driver&) {return *this;}

		public:
			static Driver& Instance () {return *_instance;}
			~Driver () {LOG ("GL-CUDA-TBB Driver destroyed");}

			virtual bool Initialize (const char* config) override;
			virtual void Run () override;
			virtual void Cleanup () override;

		protected:
			bool InitializeGLDisplay (const char* config);
			bool InitializeCUDAManager (const char* config);
			bool InitializeTBBManager (const char* config);
	};
}
