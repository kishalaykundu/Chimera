/**
 * @file AssetFactory.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The factory class for assets in the Chimera framework.
 */
#pragma once

#include <climits>
#include <map>
#include <string>
#include <memory>

#include "tinyxml2.h"
#include "Preprocess.h"

namespace Sim {

	class Asset;

	class AssetFactory {

		protected:
			static std::map <std::string, unsigned int> _componentIdMap;
			std::map <unsigned int, std::shared_ptr <Asset> > _assets;

		private: // forbidden copy constructor and assignment operator
			AssetFactory (const AssetFactory& a) {}
			AssetFactory& operator = (const AssetFactory& a) {return *this;}

		public:
			AssetFactory () {LOG ("Asset factory constructed");}
			~AssetFactory () {LOG ("Asset factory destroyed");}

			bool Initialize (const char* config);
			void Cleanup ();

			std::shared_ptr <Asset> GetAsset (unsigned int id);

			static unsigned int ComponentId (const char* name)
			{
#				ifndef NDEBUG
				auto it = _componentIdMap.find (name);
				if (it == _componentIdMap.end ()){
					LOG_ERROR ("No ID could be found for " << name);
					return UINT_MAX;
				}
				return it->second;
#				else
				return _componentIdMap [name];
#				endif
			}

		private:
			bool InitializeComponentIdMap (tinyxml2::XMLElement&);
			bool InitializeAssetMap (tinyxml2::XMLElement&);
			bool InitializeAssets (tinyxml2::XMLElement&);
			bool LoadComponents (tinyxml2::XMLElement&);
	};
}
