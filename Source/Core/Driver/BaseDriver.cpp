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

#include "Plugins/PluginManager.h"
#include "Assets/AssetFactory.h"
#include "Events/EventManager.h"
#include "Driver/BaseDriver.h"

using std::make_unique;
using tinyxml2::XMLElement;

namespace Sim {

	// default constructor
	BaseDriver::BaseDriver ()
	: _runFlag (false), _quitFlag (false) {}

	// destructor
	BaseDriver::~BaseDriver () {}

	// copy constructor (forbidden)
	BaseDriver::BaseDriver (const BaseDriver& d)
	: _runFlag (d._runFlag), _quitFlag (d._runFlag) {}

	// assignment operator (forbidden)
	BaseDriver& BaseDriver::operator = (const BaseDriver& d)
	{
		_runFlag = d._runFlag;
		_quitFlag = d._quitFlag;
		return *this;
	}

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
