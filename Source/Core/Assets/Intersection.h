/**
 * @file Intersection.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The component that processes intersections (not collisions) between
 * cutting tools and cut objects.
 */
#pragma once

#include "Assets/Component.h"

namespace Sim {
	namespace Assets {

		class Intersection : public Component {

			public:
				Intersection () {}
				virtual ~Intersection () {}

				virtual const std::string Name () const final {return "Intersection";}

				virtual bool Initialize (const char*) override {return false;}
				virtual void Update () override {}
				virtual void Cleanup () override {}
		};
	}
}
