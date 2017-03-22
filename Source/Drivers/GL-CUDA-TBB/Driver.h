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

#include "GL/GLUtils.h"

#include "Assets/AssetFactory.h"
#include "Display/GL45/GLDisplayManager.h"
#include "Events/EventManager.h"
#include "HPC/HPCManager.h"
#include "Plugins/PluginManager.h"
#include "Plugins/Plugin.h"
#include "Tasks/TaskManager.h"
#include "Driver/BaseDriver.h"

namespace Sim {

	class Driver : public BaseDriver {

		protected:
			static Driver* _instance;

		protected:
			Driver () {LOG ("GL-CUDA-TBB Driver constructed");}

		private: // forbidden copy constructor and assignment operator
			Driver (const Driver&) : BaseDriver () {}
			Driver& operator = (const Driver&) {return *this;}

		public:
			static Driver& Instance () {return *_instance;}
			~Driver () {LOG ("GL-CUDA-TBB Driver destroyed");}

			virtual bool Initialize (const char* config);
			virtual void Run ();
			virtual void Cleanup ();

			// display-related methods
			Display* GetGPUDisplay () const
			{
				return static_cast <GLDisplayManager*> (_displayManager.get ())->GetDisplay();
			}
			GLXFBConfig GetGPUConfig () const
			{
				return static_cast <GLDisplayManager*> (_displayManager.get ())->GetConfig ();
			}
			GLXContext GetGPUContext () const
			{
				return static_cast <GLDisplayManager*> (_displayManager.get ())->GetContext ();
			}
			int* GetGPUContextAttributes () const
			{
				return static_cast <GLDisplayManager*> (_displayManager.get ())->GetContextAttributes ();
			}

			GLuint AddGPUProgram (const char* name, const char* location)
			{
				return static_cast <GLDisplayManager*> (_displayManager.get ())->AddProgram (name, location);
			}
			GLuint GetGPUProgramId (const char* name) const
			{
				return static_cast <GLDisplayManager*> (_displayManager.get ())->GetProgramId (name);
			}
			bool ReloadGPUProgram (GLuint id)
			{
				return static_cast <GLDisplayManager*> (_displayManager.get ())->ReloadProgram (id);
			}

			// numerical plugin-related methods
			void AddPlugin (unsigned int id, std::shared_ptr <Plugin> p) {_pluginManager->AddPlugin (id, p);}
			std::shared_ptr <Plugin> GetPlugin (unsigned int id) const {return _pluginManager->GetPlugin (id);}
			std::shared_ptr <Plugin> GetPlugin (const char* name) const {return _pluginManager->GetPlugin (name);}

		protected:
			bool InitializeGLDisplay (const char* config);
			bool InitializeCUDAManager (const char* config);
			bool InitializeTBBManager (const char* config);
	};
}
