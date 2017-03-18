/**
 * @file PluginManager.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The plugin manager class that encapsulates all the plugins and the
 * shared libraries they are loaded from, in the Chimera Framework.
 */
#pragma once

#include <climits>
#include <string>
#include <memory>
#include <map>

#include "tinyxml2.h"
#include "Preprocess.h"

#include "Plugins/LibManager.h"

namespace Sim {

	class Plugin;

	class PluginManager {

		protected:
			std::unique_ptr <LibManager> _libManager;
			static std::map <std::string, unsigned int> _nameMap;
			std::map <unsigned int, std::shared_ptr <Plugin> > _plugins;

		private: // forbidden copy constructor and assignment operator
			PluginManager (const PluginManager&);
			PluginManager& operator = (const PluginManager&);

		public:
			PluginManager ();
			~PluginManager ();

			bool Initialize (const char* config);
			void Cleanup ();

			static unsigned int PluginId (const char* name)
			{
#				ifndef NDEBUG
				auto it = _nameMap.find (name);
				if (it == _nameMap.end ()){
					LOG_ERROR ("No ID could be found for " << name);
					return UINT_MAX;
				}
				return it->second;
#				else
				return _nameMap [name];
#				endif
			}

			void AddPlugin (unsigned int id, std::shared_ptr <Plugin> p);
			std::shared_ptr <Plugin> GetPlugin (unsigned int id);
			std::shared_ptr <Plugin> GetPlugin (const char* name);

		protected:
			bool InitializeIdMap (tinyxml2::XMLElement& elem);
	};
}
