/**
 * @file Driver.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See Driver.h.
 */
#include <cstring>
#include <memory>

#include "tinyxml2.h"
#include "Preprocess.h"
#include "InputParser.h"
#include "GLDriver/Driver.h"
#include "HPC/CUDA/CudaHPCManager.h"
#include "Tasks/TBB/TBBTaskManager.h"

using std::unique_ptr;
using std::make_unique;
using tinyxml2::XMLElement;

namespace Sim {

	Driver* Driver::_instance = new Driver ();

	bool Driver::Initialize (const char* configfile)
	{
		if (configfile == nullptr){
			LOG_ERROR ("No input configuration file specified");
			return false;
		}

		// read configuration file
		InputParser parser;
		if (!parser.Initialize (configfile, "ChimeraConfig")){
			LOG_ERROR ("Could not initialize parser for " << configfile);
			return false;
		}

		// Initialize the event manager (always the first module to be initialized)
		XMLElement* element = parser.GetElement ("EventManager");
		if (element == nullptr){
			LOG_ERROR ("Event manager profile not found in " << configfile);
			return false;
		}
		if (!InitializeEventManager (element->Attribute ("Config"))){
			Cleanup ();
			return false;
		}
		element = nullptr;

		/**
		 * Initialize renderer and display window. The input configuration may contain
		 * multiple renderer profiles. We pick the one with "OpenGL" type.
		 */
		element = parser.GetElement ("DisplayManager");
		while (element != nullptr && strcmp (element->Attribute ("Type"), "OpenGL")){
			element = element->NextSiblingElement ("DisplayManager");
		}
		if (element == nullptr){
			LOG_ERROR ("OpenGL display manager profile not found in " << configfile);
			Cleanup ();
			return false;
		}
		if (!InitializeGLDisplay (element->Attribute ("Config"))){
			Cleanup ();
			return false;
		}
		element = nullptr;

		/**
		 * Initialize HPC computing manager. The input configuration may contain
		 * multiple HPC profiles. We pick the one with "CUDA" type.
		 */
		element = parser.GetElement ("HPCManager");
		while (element != nullptr && strcmp (element->Attribute ("Type"), "CUDA")){
			element = element->NextSiblingElement ("HPCManager");
		}
		if (element == nullptr){
			LOG_ERROR ("CUDA HPC manager profile not found in " << configfile);
			Cleanup ();
			return false;
		}
		if (!InitializeCUDAManager (element->Attribute ("Config"))){
			Cleanup ();
			return false;
		}
		element = nullptr;

		// Initialize plugin manager (loads all dynamic plugin libraries)
		element = parser.GetElement ("PluginManager");
		if (element == nullptr){
			LOG_ERROR ("Plugin loader profile not found in " << configfile);
			Cleanup ();
			return false;
		}
		if (!InitializePluginManager (element->Attribute ("Config"))){
			Cleanup ();
			return false;
		}
		element = nullptr;

		exit (0);

		// Intialize asset factory and load all assets
		element = parser.GetElement ("AssetFactory");
		if (element == nullptr){
			LOG_ERROR ("Asset loader profile not found in " << configfile);
			Cleanup ();
			return false;
		}
		if (!InitializeAssetFactory (element->Attribute ("Config"))){
			Cleanup ();
			return false;
		}
		element = nullptr;

		/**
		 * Initialize Intel TBB task manager. The input configuration may contain
		 * multiple task manager profiles. We pick the one with "IntelTBB" type.
		 */
		element = parser.GetElement ("TaskManager");
		while (element != nullptr && strcmp (element->Attribute ("Type"), "IntelTBB")){
			element = element->NextSiblingElement ("TaskManager");
		}
		if (element == nullptr){
			LOG_ERROR ("Intel TBB task manager profile not found in " << configfile);
			Cleanup ();
			return false;
		}
		if (!InitializeTBBManager (element->Attribute ("Config"))){
			Cleanup ();
			return false;
		}
		element = nullptr;

		// Driver is now set to run
		_runFlag = true;

		return true;
	}

	void Driver::Run ()
	{

	}

	void Driver::Cleanup ()
	{
		_taskManager.reset ();
		_assetFactory.reset ();
		_pluginManager.reset ();
		_hpcManager.reset ();
		_displayManager.reset ();
		_eventManager.reset ();
	}

	// initialization method for GL display manager
	bool Driver::InitializeGLDisplay (const char* config)
	{
		_displayManager = make_unique <GLDisplayManager> ();
		if (!_displayManager->Initialize (config)){
			LOG_ERROR ("Display manager could not be initialized from " << config);
			return false;
		}
		return true;
	}

	// initialization method for CUDA HPC manager
	bool Driver::InitializeCUDAManager (const char* config)
	{
		_hpcManager = make_unique <CudaHPCManager> ();
		if (!_hpcManager->Initialize (config)){
			LOG_ERROR ("CUDA HPC manager could not be initialized from " << config);
			return false;
		}
		return true;
	}

	// initialization method for CUDA HPC manager
	bool Driver::InitializeTBBManager (const char* config)
	{
		_taskManager = make_unique <TBBTaskManager> ();
		if (!_taskManager->Initialize (config)){
			LOG_ERROR ("Intel TBB task manager could not be initialized from " << config);
			return false;
		}
		return true;
	}
}
