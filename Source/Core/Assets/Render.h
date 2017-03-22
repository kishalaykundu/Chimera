/**
 * @file Render.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The GPU rendering component interface for the Asset class in the
 * Chimera class. It is derived from the generic Component interface.
 * Render loads all rendering-components related to the asset. This is
 * supposed to be further customized by the specific renderer (GL/VK/DX).
 */
#pragma once

#include "Assets/Component.h"

namespace Sim {
	namespace Assets {

		class Render : public Component {

			public:
				Render () {}
				virtual ~Render () {}

				virtual const std::string Name () const final {return "Render";}

				virtual bool Initialize (tinyxml2::XMLElement& config, Asset* asset) override {return false;}
				virtual void Update () override {}
				virtual void Cleanup () override {}
		};
	}
}
