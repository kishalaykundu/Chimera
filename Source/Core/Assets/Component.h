/**
 * @file Component.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The generic component interface for the Asset class in the Chimera
 * class. Different kinds of components derive from this class. It
 * should be noted that each component is capable of loading and managing
 * specific data to be used by that component. Each asset can therefore
 * own different types of components in a mix-and-match sense.
 */
#pragma once

#include <string>
#include "tinyxml2.h"

namespace Sim {

	class Asset;

	namespace Assets {

		class Component {

				friend class Sim::Asset;

			protected:
				Asset* _owner;

			public:
				Component () : _owner (nullptr) {}
				Component (const Component& c): _owner (c._owner) {}
				Component& operator = (const Component& c) { _owner = c._owner; return *this;}
				virtual ~Component () {_owner = nullptr;}

				virtual const std::string Name () const = 0;

				virtual bool Initialize (tinyxml2::XMLElement& config, Asset* asset) = 0;
				virtual void Update () = 0;
				virtual void Cleanup () = 0;
		};
	}
}
