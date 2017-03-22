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

#include "CuglMsd.h"
#include "CuglMsdRender.h"
#include "CuglMsdPhysics.h"

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

	bool CuglMsd::InitializeAssetComponent (const char* componentName, XMLElement& config, Asset* asset)
	{
		if (!strcmp ("Geometry", componentName)){
			return InitializeGeometry (config, asset);
		}
		else if (!strcmp ("Render", componentName)){
			return InitializeRender (config, asset);
		}
		else if (!strcmp ("Physics", componentName)){
			return InitializePhysics (config, asset);
		}
		LOG_ERROR ("Invalid component " << componentName);
		return false;
	}

	void CuglMsd::Cleanup ()
	{ }

	// initialize geometry component (using generic definition of Geometry from Asset folder)
	bool CuglMsd::InitializeGeometry (XMLElement& config, Asset* asset)
	{
		shared_ptr <Assets::Geometry> gc = make_shared <Assets::Geometry> ();
		if (!gc->Initialize (config, asset)){
			LOG_ERROR ("Could not initialize geometry component");
			return false;
		}
		asset->AddComponent ("Geometry", gc);
		return true;
	}

	bool CuglMsd::InitializeRender (XMLElement& config, Asset* asset)
	{
		shared_ptr <Assets::CuglMsdRender> rc = make_shared <Assets::CuglMsdRender> ();
		if (!rc->Initialize (config, asset)){
			LOG_ERROR ("Could not initialize render component");
			return false;
		}
		asset->AddComponent ("Render", rc);
		return true;
	}

	bool CuglMsd::InitializePhysics (XMLElement& config, Asset* asset)
	{
		shared_ptr <Assets::CuglMsdPhysics> pc = make_shared <Assets::CuglMsdPhysics> ();
		if (!pc->Initialize (config, asset)){
			LOG_ERROR ("Could not initialize physics component");
			return false;
		}
		return true;
	}
}
