/**
 * @file BaseDriver.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The application interface class for the Chimera framework. This is
 * the application layer that is in charge of interfacing with essential
 * Chimera subsysems and all device and OS-specific services. It should
 * also initialize all the necessary components of the simulation to be
 * called during runtime.
 * This class cannot be used in itself. It provides all the functional
 * components common to any driver. Platform-specific driver classes have
 * to be derived from this. This design is meant to make it easy for the
 * platform specific stuff to happen without re-coding a lot of stuff.
 * Platform specific stuff includes graphics packages OpenGL, physics
 * packages like CUDA, thread-scheduling packages like Intel TBB.
 */
#pragma once

#include <memory>

namespace Sim {

	class AssetFactory;
	class DisplayManager;
	class EventManager;
	class HPCManager;
	class PluginManager;
	class TaskManager;

	class BaseDriver {

		protected:
			bool _runFlag;
			bool _quitFlag;

			/**
			 * The Plugin manager. This is responsible for all the dynamically loaded
			 * plugin libraries. This in effect serves as the logic layer manager for
			 * the Chimera system.
			 */
			std::unique_ptr <PluginManager> _pluginManager;
			/**
			 * The Asset factory that contains pointers to every persistent asset in
			 * the Chimera system. All assets are accessed through the asset factory.
			 */
			std::unique_ptr <AssetFactory> _assetFactory;
			/**
			 * The Event manager for the whole simulation system. This is used by all
			 * subsystems to pass messages and synchronize with each other.
			 */
			std::unique_ptr <EventManager> _eventManager;
			/**
			 * The Display/Rendering/Windowing system manager for the Chimera system.
			 * Every platform like OpenGL, Vulkan etc. will have their own unique im-
			 * plementation for this module.
			 */
			std::unique_ptr <DisplayManager> _displayManager;
			/**
			 * The High Performance Computing manager for the Chimera system. It is
			 * tasked with controlling the parallel numerical computing subsystems,
			 * typically provided by the plugins
			 */
			std::unique_ptr <HPCManager> _hpcManager;
			/**
			 * The Task Scheduler for the Chimera system. This manages the running loop
			 * of the simulation. This provides the logic for the exact kind of schedu-
			 * ling being done whether it be through threadpool, or Intel TBB, or some
			 * similar thing.
			 */
			std::unique_ptr <TaskManager> _taskManager;

		protected:
			BaseDriver ();
			virtual ~BaseDriver ();

		private: // forbidden copy ctor. and assignment operator
			BaseDriver (const BaseDriver&);
			BaseDriver& operator = (const BaseDriver&);

		public:
			virtual bool Initialize (const char* configfile) = 0;
			virtual void Run () = 0;
			virtual void Cleanup () = 0;
			virtual void Quit ();

		protected:
			virtual bool InitializePluginManager (const char* config);
			virtual bool InitializeAssetFactory (const char* config);
			virtual bool InitializeEventManager (const char* config);
	};
}
