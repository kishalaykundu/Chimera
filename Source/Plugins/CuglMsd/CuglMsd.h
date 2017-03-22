/**
 * @file CuglMsd.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The OpenGL-CUDA based Mass spring damper plugin for the Chimera
 * system.
 */
#pragma once

#include <memory>
#include "tinyxml2.h"

#include "Plugins/Plugin.h"

namespace Sim {

	class Asset;

	class EXPORT CuglMsd : public Plugin {

	public:
		CuglMsd ();
		~CuglMsd ();

		virtual const char* Name () const override {return "CuglMsd";}

		virtual bool InitializeAssetComponent (const char* componentName, tinyxml2::XMLElement& config, Asset* asset) override;
		virtual void Cleanup () override;

	protected:
		bool InitializeGeometry (tinyxml2::XMLElement& config, Asset* asset);
		bool InitializeRender (tinyxml2::XMLElement& config, Asset* asset);
		bool InitializePhysics (tinyxml2::XMLElement& config, Asset* asset);
	};
}
