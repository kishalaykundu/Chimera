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

#include "InputParser.h"
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
			_components [cid]->_owner = const_cast <Asset*> (this);

			clist = clist->NextSiblingElement ("Component");
		}

		LOG ("Asset \'" << name << "\' initialized");
		return true;
	}

	void Asset::Cleanup ()
	{
		_components.clear ();
	}

	bool Asset::LoadComponents (XMLElement& elem)
	{
		// get the name of the config file for the asset
		const char* config = elem.Attribute ("Config");
		if (config == nullptr){
			LOG_ERROR ("No config file specified for " << elem.Attribute ("Name"));
			return false;
		}

		InputParser parser;
		if (!parser.Initialize (config, "AssetConfig")){
			LOG_ERROR ("Could not initialize parser for " << config);
			return false;
		}

		XMLElement* clist = elem.FirstChildElement ("Component");

		while (clist != nullptr){
			const char* type = clist->Attribute ("Type");
			const char* plugin = clist->Attribute ("LoadingPlugin");
			if (plugin == nullptr){
				LOG_ERROR ("No loading plugin specified for " << elem.Attribute ("Name") << "\'s " << type);
				return false;
			}

			XMLElement* telem = parser.GetElement (type);
			if (telem == nullptr){
				LOG_ERROR ("No specification for " << type << " found in " << config);
				return false;
			}

			shared_ptr <Plugin> p = Driver::Instance ().GetPlugin (plugin);
			if (!p){
				LOG_ERROR ("Plugin " << plugin << " not found");
				return false;
			}
			if (!p->InitializeAssetComponent (type, *telem, const_cast <Asset*> (this))){
				LOG_ERROR ("Could not initialize " << type << " component from " << config);
				return false;
			}

			clist = clist->NextSiblingElement ("Component");
		}
		return true;
	}
}
