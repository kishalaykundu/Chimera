/**
 * @file LibManager.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See LibManager.h
 */

#include <cstring>
#include <map>
#include <string>
#include <memory>

#include "Preprocess.h"

#include "Plugins/SharedLib.h"
#include "Plugins/LibManager.h"
#include "Plugins/PluginManager.h"
#include "Plugins/Plugin.h"

using std::map;
using std::string;
using std::unique_ptr;
using std::make_unique;
using tinyxml2::XMLElement;
using tinyxml2::XMLError;

namespace Sim {

	void GetRawLibName (const string& fullname, string& result)
	{
		auto const pos1 = fullname.find_last_of ('/');
		auto const pos2 = fullname.find_last_of ('.') - 1;

		// get the name of the library without location or suffix
		string substr = fullname.substr (pos1 + 1, pos2 - pos1);

		// for non-windows platform take out the 'lib' prefix
#		ifndef _WIN32
		auto const pos3 = substr.find ("lib") + 3;
#		else
		auto const pos3 = pos1;
#		endif

		// take out trailing debug for non-release libraries
#		ifndef NDEBUG
		auto const pos4 = substr.find ("-debug");
#		else
		auto const pos4 = pos2;
#		endif

		result = substr.substr (pos3, pos4 - pos3);
	}

	LibManager::LibManager ()
	{
		LOG ("Library manager constructed");
	}

	LibManager::~LibManager ()
	{
		if (!_libs.empty ()){
			Cleanup ();
		}
		LOG ("Plugin shared library manager destroyed.");
	}

	LibManager::LibManager (const LibManager& lm) {}

	LibManager& LibManager::operator = (const LibManager& lm) {return *this;}

	bool LibManager::Initialize (XMLElement& elem)
	{
		unsigned int count = 0;
		XMLError error = tinyxml2::XML_SUCCESS;
		if ((error = elem.QueryUnsignedAttribute ("Count", &count)) != tinyxml2::XML_SUCCESS){
			LOG_ERROR (elem.Value () << " does not have plugin-count attribute");
			return false;
		}

		const XMLElement* liblist = elem.FirstChildElement ("Plugin");
		if (liblist == nullptr){
			LOG_ERROR (elem.Value () << "has no \'Plugin\' members");
			return false;
		}
		while (liblist != nullptr){

			const char* name = nullptr;
			name = liblist->Attribute ("Name");
#			ifndef NDEBUG
			if (name == nullptr){
				LOG_ERROR ("The \'" << liblist->Value () << "\' member of \'" << elem.Value () <<"\' has no attribute called \'Name\'");
				return false;
			}
#			endif
			string libNameWithPath;
			const char* libfolder = nullptr;
			libfolder = liblist->Attribute ("Location");
			if (libfolder != nullptr){
				libNameWithPath = libfolder;
				if (libNameWithPath [libNameWithPath.size () - 1] != '/'){
					libNameWithPath += "/";
				}
			}
			const char* suffix = nullptr;
#			if defined (__linux__)
			libNameWithPath += "lib";
			suffix = ".so";
#			elif defined (__APPLE__)
			libNameWithPath += "lib";
			suffix = ".dylib";
#			elif defined (_WIN32)
			suffix = ".dll";
#			endif
			libNameWithPath += name;
#			ifndef NDEBUG
			libNameWithPath += "-debug";
#			endif
			libNameWithPath += suffix;

			AddLibrary (libNameWithPath);
			liblist = liblist->NextSiblingElement ("Plugin");
		}
		if (!LoadAllLibraries ()){
			return false;
		}
		LOG ("Library manager initialized");
		return true;
	}

	void LibManager::Cleanup ()
	{
		for (auto &lib : _libs){
			if (!lib.second->Unload (lib.first)){
				LOG_WARNING (lib.first << "could not be unloaded");
			}
			_libs.erase (lib.first);
		}
	}

	void LibManager::AddLibrary (const string& fullLibName)
	{
		// check for duplicates and return with message if true
#		ifndef NDEBUG
		auto lib = _libs.find (fullLibName);
		if (lib != _libs.end ()){
			LOG_WARNING (fullLibName << " already exists. Library not added");
			return;
		}
#		endif
		unique_ptr <SharedLib> p;
		_libs [fullLibName] = std::move (p);
	}

	bool LibManager::LoadAllLibraries ()
	{
		for (auto &l : _libs) {
			l.second = make_unique <SharedLib> ();
			if (!l.second->Load (l.first)){
				LOG_ERROR ("Could not load " << l.first);
				_libs.erase (l.first);
				return false;
			}
#			ifdef __GNUC__
			__extension__
#			endif
			NewPlugin start = (NewPlugin)(l.second->GetSymbol ("StartPlugin"));
			if (!start){
				LOG_ERROR ("Function \'StartPlugin\' not found in " << l.first);
				return false;
			}
			string name;
			GetRawLibName (l.first, name);
			start (PluginManager::PluginId (name.c_str ())); // this function adds the corresponding plugins to PluginManager
		}
		return true;
	}
}
