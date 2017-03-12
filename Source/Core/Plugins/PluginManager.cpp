/**
 * @file PluginManager.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See PluginManager.h.
 */

#include <string>
#include <memory>
#include <map>

#include "Preprocess.h"
#include "InputParser.h"

#include "Plugins/LibManager.h"
#include "Plugins/PluginManager.h"
#include "Plugins/Plugin.h"

using std::string;
using std::shared_ptr;
using std::make_unique;
using tinyxml2::XMLElement;
using tinyxml2::XMLError;
using tinyxml2::XML_SUCCESS;

namespace Sim {

	std::map <std::string, unsigned int> PluginManager::_nameMap;

	PluginManager::PluginManager ()
	{
		LOG ("Plugin manager constructed");
	}

	PluginManager::~PluginManager ()
	{
		Cleanup ();
		LOG ("Plugin manager destroyed");
	}

	PluginManager::PluginManager (const PluginManager& pm) {}

	PluginManager& PluginManager::operator = (const PluginManager& pm) {return *this;}

	bool PluginManager::Initialize (const char* configfile)
	{
		InputParser parser;
		if (!parser.Initialize (configfile, "PluginsConfig")){
			LOG_ERROR ("Could not initialize parser for " << configfile);
			return false;
		}

		// Initialize the name-to-id mapper
		XMLElement* element = parser.GetElement ("PluginIdMap");
		if (element == nullptr){
			LOG_ERROR ("No id mapper specified in " << configfile);
			return false;
		}
		if (!InitializeIdMap (*element)){
			LOG_ERROR ("Could not initialize plugin id mapper from " <<  configfile);
			Cleanup ();
			return false;
		}
		element = nullptr;

		/*
		 * Initialize the library manager and add all the plugin libraries.
		 * At this stage the plugins are not loaded, only the libraries. This
		 * is done to maintain memory congruity.
		 */
		element = parser.GetElement ("Plugins");
		if (element == nullptr){
			LOG_ERROR ("No plugin libraries specified in " << configfile);
			Cleanup ();
			return false;
		}

		_libManager = make_unique <LibManager> ();
		if (!_libManager->Initialize (*element)){
			LOG_ERROR ("Plugin library manager failed to initialize from " << configfile);
			Cleanup ();
			return false;
		}
		LOG ("Plugin manager initialized");
		return true;
	}

	void PluginManager::Cleanup ()
	{
		_plugins.clear ();
		_libManager.clear ();
		_nameMap.clear ();
	}

	void PluginManager::AddPlugin (unsigned int id, shared_ptr <Plugin> p)
	{
		_plugins [id] = p;
	}

	shared_ptr <Plugin> PluginManager::GetPlugin (unsigned int id)
	{
#		ifndef NDEBUG
		auto p = _plugins.find (id);
		if (p != _plugins.end ()){
			return p->second;
		}

		LOG_WARNING ("Could not find plugin with ID " << id);
		shared_ptr <Plugin> dp;
		return dp;
#		else
		return _plugins [id];
#		endif
	}

	shared_ptr <Plugin> PluginManager::GetPlugin (const char* name)
	{
#		ifndef NDEBUG
		auto p = _nameMap.find (name);
		if (p != _nameMap.end ()){
			auto pi = _plugins.find (p->second);
			if (pi != _plugins.end ()){
				return pi->second;
			}
		}

		LOG_WARNING ("Could not find plugin " << name);
		shared_ptr <Plugin> dp;
		return dp;
#		else
		return _plugins [ _nameMap [name]];
#		endif
	}

	bool PluginManager::InitializeIdMap (XMLElement& elem)
	{
		XMLElement* melem = elem.FirstChildElement ("PluginIdMap");
		if (melem == nullptr){
			LOG_ERROR ("No plugin Id mapper specified in config");
			return false;
		}

		XMLElement* mlist = melem->FirstChildElement ("Map");
		if (mlist == nullptr){
			LOG_ERROR ("No plugin Id maps specified in config");
			return false;
		}

		XMLError error = XML_SUCCESS;
		while (mlist != nullptr){
			const char* name = mlist->Attribute ("Name");
			if (name == nullptr){
				LOG_ERROR ("No \'Name\' attribute found in plugin Id map");
				return false;
			}
			auto f = _nameMap.find (name);
			if (f != _nameMap.end ()){
				LOG_ERROR ("Duplicate entry for " << name << " in plugin Id map (forbidden)");
				return false;
			}
			unsigned int key = 0;
			if ((error = mlist->QueryUnsignedAttribute ("Key", &key)) != XML_SUCCESS){
				LOG_ERROR ("Could not read key value for " << name << " in plugin Id map");
				return false;
			}
			for (auto &k : _nameMap){
				if (k.second == key){
					LOG_ERROR ("Duplicate id found for " << name << "in plugin Id map (forbidden)");
					return false;
				}
			}
			_nameMap [name] = key;
			mlist = mlist->NextSiblingElement ("Map");
		}
		return true;
	}

}
