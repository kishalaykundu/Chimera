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
#include <memory>

namespace Sim {

	class AssetFactory;
	class Asset;

	namespace Assets {

		class Component {

				friend class Sim::AssetFactory;

			protected:
				std::shared_ptr <Asset> _owner;

			public:
				Component () {}
				Component (const Component& c): _owner (c._owner) {}
				Component& operator = (const Component& c) { _owner = c._owner; return *this;}
				virtual ~Component () {_owner.reset ();}

				virtual const std::string Name () const = 0;

				virtual bool Initialize (const char*) = 0;
				virtual void Update () = 0;
				virtual void Cleanup () = 0;

			protected:
				void SetOwner (std::shared_ptr <Asset> a) {_owner = a;}
		};
	}
}
