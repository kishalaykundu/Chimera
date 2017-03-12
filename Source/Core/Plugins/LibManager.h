/**
 * @file LibManager.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The manager class for the plugins libraries in the Chimera framework.
 * This class manages all the loaded plugin libraries. These libraries
 * mostly define the various implementations of the plugin interface for
 * the framework. All meaningful computations in Chimera are handled by
 * custom-defined plugin libraries.
 */
#pragma once

#include <map>
#include <string>
#include <memory>

#include "tinyxml2.h"
#include "Plugins/SharedLib.h"

namespace Sim {

	class LibManager {

		protected:
			std::map <std::string, std::unique_ptr <SharedLib> > _libs;

		private: // forbidden copy constructor and assignment operator
			LibManager (const LibManager&);
			LibManager& operator = (const LibManager&);

		public:
			LibManager ();
			~LibManager ();

			bool Initialize (tinyxml2::XMLElement&);
			void Cleanup ();

		private:
			void AddLibrary (const std::string& fullLibName);
			bool LoadAllLibraries ();
	};
}
