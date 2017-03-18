/**
 * @file CuglMsd.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See CuglMsd.h.
 */

#include <cstring>
#include <memory>
#include "tinyxml2.h"

#include "Preprocess.h"
#include "InputParser.h"
#include "Driver.h"

#include "Assets/Asset.h"
#include "Assets/Component.h"
#include "Assets/Geometry.h"
#include "Assets/Render.h"
#include "Assets/Physics.h"
#include "CuglMsd.h"

using std::shared_ptr;
using std::make_shared;
using std::static_pointer_cast;

using tinyxml2::XMLElement;

namespace Sim {

	extern "C" EXPORT void
	StartPlugin (unsigned int id)
	{
		shared_ptr <CuglMsd> plugin = make_shared <CuglMsd> ();
		Driver::Instance().AddPlugin (id, shared_ptr <Plugin> (plugin));
	}

	EXPORT CuglMsd::CuglMsd ()
	{
		LOG ("CuglMsd plugin constructed");
	}
	EXPORT CuglMsd::~CuglMsd ()
	{
		LOG ("CuglMsd plugin destroyed");
	}

	bool CuglMsd::InitializeAssetComponent (const char* componentName, const char* config, shared_ptr <Asset>& asset)
	{
		InputParser parser;
		if (!parser.Initialize (config, "MsdConfig")){
			LOG_ERROR ("Could not initialize parser for " << config);
			return false;
		}

		XMLElement* element = parser.GetElement (componentName);
		if (element != nullptr){
			LOG_ERROR (componentName << " component not specified in " << config);
			return false;
		}
		const char* configfile = element->Attribute ("Config");
		if (config == nullptr){
			LOG_ERROR ("Config file not specified for " << componentName);
			return false;
		}

		if (!strcmp ("Geometry", componentName)){
			return InitializeGeometry (configfile, asset);
		}
		else if (!strcmp ("Render", componentName)){
			return InitializeRender (configfile, asset);
		}
		else if (!strcmp ("Physics", componentName)){
			return InitializePhysics (configfile, asset);
		}
		LOG_ERROR ("Invalid component " << componentName);
		return false;
	}

	void CuglMsd::Cleanup ()
	{ }

	// initialize geometry component (using generic definition of Geometry from Asset folder)
	bool CuglMsd::InitializeGeometry (const char* config, std::shared_ptr <Asset>& asset)
	{
		shared_ptr <Assets::Geometry> gc = make_shared <Assets::Geometry> ();
		if (!gc->Initialize (config)){
			LOG_ERROR ("Could not initialize geometry component from " << config);
			return false;
		}
		asset->AddComponent ("Geometry", static_pointer_cast <Assets::Component> (gc));
		return true;
	}

	bool CuglMsd::InitializeRender (const char* config, std::shared_ptr <Asset>& asset)
	{
		return true;
	}

	bool CuglMsd::InitializePhysics (const char* config, std::shared_ptr <Asset>& asset)
	{
		return true;
	}
}
