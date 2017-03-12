/**
 * @file AssetFactory.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See AssetFactory.h.
 */

#include <map>
#include <memory>

#include "tinyxml2.h"
#include "Preprocess.h"

#include "Driver.h"

#include "Assets/Asset.h"
#include "Assets/AssetFactory.h"

using std::map;
using std::shared_ptr;
using std::make_shared;
using tinyxml2::XMLElement;
using tinyxml2::XMLError;
using tinyxml2::XML_SUCCESS;

namespace Sim {

	std::map <std::string, unsigned int> AssetFactory::_componentIdMap;

	bool AssetFactory::Initialize (const char* configfile)
	{
		// read configuration file
		InputParser parser;
		if (!parser.Initialize (configfile, "AssetsConfig")){
			LOG_ERROR ("Could not initialize parser for " << configfile);
			return false;
		}

		XMLElement* element = parser.GetElement ("ComponentMap");
		if (element == nullptr){
			LOG_ERROR ("No component Id Map specified in " << configfile);
			return false;
		}
		if (!InitializeComponentIdMap (*element)){
			LOG_ERROR ("Asset component Id Map could not be initialized from " << configfile);
			Cleanup ();
			return false;
		}
		element = nullptr;

		element = parser.GetElement ("Assets");
		if (element == nullptr){
			LOG_ERROR ("No assets specified in " << configfile);
			Cleanup ();
			return false;
		}
		if (!InitializeAssetMap (*element)){
			LOG_ERROR ("Asset map could not be initialized from " << configfile);
			Cleanup ();
			return false;
		}
		if (!InitializeAssets (*element)){
			LOG_ERROR ("Failed to initialize all assets from " << configfile);
			Cleanup ();
			return false;
		}
		if (!LoadComponents (elem)){
			LOG_ERROR ("Failed to load all asset components from " << configfile);
			Cleanup ();
			return false;
		}
		LOG ("AssetFactory initialized");
		return true;
	}

	void AssetFactory::Cleanup ()
	{
		_assets.clear ();
		_componentIdMap.clear ();
	}

	shared_ptr <Asset> AssetFactory::GetAsset (unsigned int id)
	{
#		ifndef NDEBUG
		auto f = _assets.find (id);
		if (f != _assets.end ()){
			return f->second;
		}
		shared_ptr <Asset> a;
		LOG_ERROR ("Could not find asset with id " << id << ". Returning empty asset");
		return a;
#		else
		return _assets [id];
#		endif
	}

	bool AssetFactory::InitializeComponentIdMap (XMLElement& elem)
	{
		const XMLElement* clist = elem.FirstChildElement ("Map");
		if (clist == nullptr){
			LOG_ERROR (elem.Value () << "has no \'Map\' members");
			return false;
		}

		XMLError error = XML_SUCCESS;
		while (clist != nullptr){
			const char* name = clist->Attribute ("Name");
			if (name == nullptr){
				LOG_ERROR ("Map element in ComponentIdMap has no name");
				return false;
			}
			unsigned int key = 0;
			if ((error = clist->QueryUnsignedAttribute ("Key", &key)) != XML_SUCCESS){
				LOG_ERROR ("No key value associated with Map element" << name);
				return false;
			}
			_componentIdMap [name] = key;
			clist = clist->NextSiblingElement ("Map");
		}
		return true;
	}

	bool AssetFactory::InitializeAssetMap (XMLElement& elem)
	{
		const XMLElement* alist = elem.FirstChildElement ("Asset");
		if (alist == nullptr){
			LOG_ERROR (elem.Value () << "has no \'Asset\' members");
			return false;
		}

		XMLError error = XML_SUCCESS;
		while (alist != nullptr){

			const char* name = nullptr;
			name = alist->Attribute ("Name");
			if (name == nullptr){
				LOG_ERROR ("The \'" << alist->Value () << "\' member of \'" << elem.Value () <<"\' has no attribute called \'Name\'");
				return false;
			}

			unsigned int id = 0;
			if ((error = alist->QueryUnsignedAttribute ("ID", &id)) != XML_SUCCESS){
				LOG_ERROR ("Error reading ID for asset \'" << name << "\'");
				return false;
			}
			auto f = _assets.find (id);
			if (f != _assets.end ()){
				LOG_ERROR ("Duplicate asset id " << id << "found in config file (forbidden)");
				return false;
			}

			// add dummy asset ptr to map (this results in near-contiguous map)
			shared_ptr <Asset> p;
			_assets [id] = p;

			alist = alist->NextSiblingElement ("Asset");
		}
		return true;
	}

	bool AssetFactory::InitializeAssets (XMLElement& elem)
	{
		XMLElement* alist = elem.FirstChildElement ("Asset");

		while (alist != nullptr){

			const char* name = alist->Attribute ("Name");

			const char* assettype = nullptr;
			assettype = alist->Attribute ("Type");
			if (assettype == nullptr){
				LOG_ERROR ("No type specified for Asset \'" << name);
				return false;
			}

			unsigned int id = alist->UnsignedAttribute ("ID");
			_assets [id] = make_shared <Asset> (id, assettype);

			_assets [id]->Initialize (*alist);

			alist = alist->NextSiblingElement ("Asset");
		}
		return true;
	}

	bool AssetFactory::LoadComponents (XMLElement& elem)
	{
		XMLElement* alist = elem.FirstChildElement ("Asset");

		while (alist != nullptr){
			auto a = _assets.find (alist->UnsignedAttribute ("ID"));

			XMLElement* clist = alist->FirstChildElement ("Component");
			while (clist != nullptr){
				const char* type = clist->Attribute ("Type");
				const char* plugin = clist->Attribute ("LoadingPlugin");
				if (plugin == nullptr){
					LOG_ERROR ("No loading plugin specified for " << alist->Attribute ("Name") << "\'s " << type);
					return false;
				}
				const char* config = clist->Attribute ("Config");

				shared_ptr <Assets::Component> cp = Engine::Instance ().GetPlugin (plugin)->Initialize (type, config);
				unsigned int cid = ComponentId (type);
				a->second->_components [cid] = std::move (cp);
				a->second->_components [cid]->SetOwner (a->second);

				clist = clist->NextSiblingElement ("Component");
			}

			alist = alist->NextSiblingElement ("Asset");
		}
		return true;
	}

}
