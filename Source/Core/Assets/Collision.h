/**
 * @file Collision.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The collision component interface for the Asset class in the
 * Chimera class. It is derived from the generic Component interface.
 * Collision loads all rendering-components related to the asset. This
 * is supposed to be further customized by the specific collision module.
 */
#pragma once

#include "Assets/Component.h"

namespace Sim {
	namespace Assets {

		class Collision : public Component {

			public:
				Collision () {}
				virtual ~Collision () {}

				virtual const std::string Name () const final {return "Collision";}

				virtual bool Initialize (const char*) override {return false;}
				virtual void Update () override {}
				virtual void Cleanup () override {}
		};
	}
}
