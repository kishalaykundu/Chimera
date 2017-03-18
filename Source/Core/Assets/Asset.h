/**
 * @file Asset.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The asset class for the Chimera system, representing all simulated
 * bodies/entities. Its a component based system, since different bodies
 * are compositions (mix) of distinct components, that may or may not
 * be similar across different objects.
 */
#pragma once

#include <map>
#include <string>
#include <memory>

#include "tinyxml2.h"
#include "Preprocess.h"
#include "Assets/AssetFactory.h"
#include "Assets/Component.h"

namespace Sim {

	class EXPORT Asset {

			friend class AssetFactory;

		protected:
			unsigned int _id;
			std::string _type;
			std::map <unsigned int, std::shared_ptr <Assets::Component> > _components;

		private: // forbidden constructors and assignment operator
			Asset (): _id (0) {}
			Asset (const Asset& a): _id (a._id), _type (a._type) {}
			Asset& operator = (const Asset& a) {_type = a._type; return *this;}

		public:
			Asset (unsigned int id, const std::string& type): _id (id), _type (type) {}
			~Asset () {Cleanup ();}

			const std::string& Type () const {return _type;}
			unsigned int Id () const {return _id;}

			bool Initialize (tinyxml2::XMLElement&);
			void Cleanup ();

			void AddComponent (const char* name, std::shared_ptr <Assets::Component> component)
			{
				_components [AssetFactory::ComponentId (name)] = component;
			}

			template <class ComponentType> std::weak_ptr <ComponentType> GetComponent (const char* name)
			{
				unsigned int id = AssetFactory::ComponentId (name);
				auto it = _components.find (id);
				if (it != _components.end ()){
					std::shared_ptr <Assets::Component> genericComponent (it->second);
					std::shared_ptr <ComponentType> component (std::static_pointer_cast <ComponentType> (genericComponent));
					return std::weak_ptr <ComponentType> (component);
				}
				return std::weak_ptr <ComponentType> ();
			}

			template <class ComponentType> std::weak_ptr <ComponentType> GetComponent (unsigned int id)
			{
				auto it = _components.find (id);
				if (it != _components.end ()){
					std::shared_ptr <Assets::Component> genericComponent (it->second);
					std::shared_ptr <ComponentType> component (std::static_pointer_cast <ComponentType> (genericComponent));
					return std::weak_ptr <ComponentType> (component);
				}
				return std::weak_ptr <ComponentType> ();
			}

	};
}
