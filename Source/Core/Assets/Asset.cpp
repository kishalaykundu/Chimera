/**
 * @file Asset.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See Asset.h
 */

#include <map>
#include <memory>

#include "tinyxml2.h"
#include "Preprocess.h"

#include "Driver.h"
#include "Plugins/Plugin.h"

#include "Assets/Asset.h"
#include "Assets/Component.h"

using std::string;
using std::shared_ptr;
using std::make_shared;
using tinyxml2::XMLElement;
using Sim::Assets::Component;

namespace Sim {

	bool Asset::Initialize (XMLElement& elem)
	{
		const char* name = elem.Attribute ("Name");

		XMLElement* clist = elem.FirstChildElement ("Component");
		if (clist == nullptr){
			LOG_ERROR ("No components specified for \'" << name << "\'");
			return false;
		}

		while (clist != nullptr){

			const char* type = clist->Attribute ("Type");
			if (type == nullptr){
				LOG_ERROR ("No type specified for component for \'" << name << "\'");
				return false;
			}
			unsigned int cid = AssetFactory::ComponentId (type);

			auto t = _components.find (cid);
			if (t != _components.end ()){
				LOG_ERROR ("Duplicate component of type \'" << type << "\' specified for " << name << " (forbidden)");
				return false;
			}
			shared_ptr <Component> c;
			_components [cid] = c;

			clist = clist->NextSiblingElement ("Component");
		}

		LOG ("Asset \'" << name << "\' initialized");
		return true;
	}

	void Asset::Cleanup ()
	{
		_components.clear ();
	}

}
