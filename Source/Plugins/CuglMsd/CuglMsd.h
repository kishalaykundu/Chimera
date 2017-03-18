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

#include "Plugins/Plugin.h"
#include "Assets/Component.h"

namespace Sim {

	class CuglMsd : public Plugin {

	public:
		CuglMsd ();
		~CuglMsd ();

		virtual const char* Name () const override {return "CuglMsd";}

		virtual bool InitializeAssetComponent (const char* componentName, const char* config, std::shared_ptr <Asset>& asset) override;
		virtual void Cleanup () override;

	protected:
		bool InitializeGeometry (const char* config, std::shared_ptr <Asset>& asset);
		bool InitializeRender (const char* config, std::shared_ptr <Asset>& asset);
		bool InitializePhysics (const char* config, std::shared_ptr <Asset>& asset);
	};
}
