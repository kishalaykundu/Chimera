/**
 * @file UIDGenerator.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 * A global unique identifier generator system.
 */
#pragma once

#include <random>
#include <mutex>

namespace Sim {

	class UIDGenerator {

		private:
			std::mutex _mutex;
			static UIDGenerator* _instance;

			UIDGenerator () // private constructor
			{

			}
			UIDGenerator (const UIDGenerator& g) {} // forbidden copy constructor
			UIDGenerator& operator = (const UIDGenerator& g) // forbidden assignment operator
			{
				return *this;
			}

		public:
			~UIDGenerator () {}
			static UIDGenerator& Instance ()
			{
				if (_instance) {
					return *_instance;
				} else {
					_instance = new UIDGenerator ();
				}
				return *_instance;
			}

			unsigned int GetUniqueId ();
	};
}
