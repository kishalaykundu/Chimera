/**
 * @file Plugin.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The plugin interface for the driver in the Simulate framework. The
 * actual implementation is left to the individual implementation
 * libraries. Each plugin is responsible for implementing a specific
 * kind of task/algorithm.
 */
#pragma once

#include <memory>

namespace Sim {

	namespace Assets {
		class Component;
	}

	class Plugin {

		private: // forbidden constructors and operator
			Plugin (const Plugin& p) {}
			Plugin& operator = (const Plugin& p) {return *this;}

		public:
			Plugin () {}
			virtual ~Plugin () {}

			virtual const char* Name () const = 0;

			virtual std::shared_ptr <Assets::Component> Initialize (const char* componentName, const char* config) = 0;
			virtual void Cleanup () = 0;
	};

	typedef void (*NewPlugin)(unsigned int);

	// interface for constructor from dynamically loaded libraries.
	extern "C" EXPORT void StartPlugin (unsigned int);
}
