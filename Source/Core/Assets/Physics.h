/**
 * @file Physics.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The physics component interface for the Asset class in the Chimera
 * class. It is derived from the generic Component interface. Physics
 * loads all physics-based components related to the asset. This is
 * supposed to be further customized by the specific physics module.
 */
#pragma once

#include "Assets/Component.h"

namespace Sim {
	namespace Assets {

		class Physics : public Component {

			public:
				Physics () {}
				virtual ~Physics () {}

				virtual const std::string Name () const final {return "Physics";}

				virtual bool Initialize (tinyxml2::XMLElement& config, Asset* asset) override {return false;}
				virtual void Update () override {}
				virtual void Cleanup () override {}
		};
	}
}
