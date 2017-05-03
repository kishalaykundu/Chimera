/**
 * @file BaseDriver.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See BaseDriver.h
 */

#include <memory>

#include "Config.h"
#include "Preprocess.h"

#include "Display/DisplayManager.h"
#include "HPC/HPCManager.h"
#include "Tasks/TaskManager.h"
#include "Plugins/PluginManager.h"
#include "Assets/AssetFactory.h"
#include "Events/EventManager.h"
#include "Driver/BaseDriver.h"

using std::make_unique;
using tinyxml2::XMLElement;

namespace Sim {

	// prepare for quitting
	void BaseDriver::Quit () {_runFlag = false;}

	bool BaseDriver::InitializePluginManager (const char* config)
	{
		_pluginManager = make_unique <PluginManager> ();
		if (!_pluginManager->Initialize (config)){
			LOG_ERROR ("All plugin libraries specified in " << config << " could not be initialized");
			return false;
		}
		return true;
	}

	bool BaseDriver::InitializeAssetFactory (const char* config)
	{
		_assetFactory = make_unique <AssetFactory> ();
		if (!_assetFactory->Initialize (config)){
			LOG_ERROR ("All assets specified in " << config << " could not be initialized");
			return false;
		}
		return true;
	}

	bool BaseDriver::InitializeEventManager (const char* config)
	{
		_eventManager = make_unique <EventManager> ();
		if (!_eventManager->Initialize (config)){
			LOG_ERROR ("Event Manager could not be initialized from " <<  config);
			return false;
		}
		return true;
	}
}
