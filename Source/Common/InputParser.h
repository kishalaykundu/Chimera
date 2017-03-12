/**
 * @file InputParser.h
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
 * The XML-styled configuration file parser for the Chimera framework.
 */
#pragma once

#include "tinyxml2.h"

namespace Sim {

	class InputParser {
		private:
			tinyxml2::XMLDocument _doc;
			tinyxml2::XMLNode* _root;

		public:
			InputParser (): _root (nullptr) {}
			~InputParser () {}

			/**
			 * Argument1: XML file to be read
			 * Argument2: Name of the root element of the file
			 */
			bool Initialize (const char*, const char*);
			const char* DocName () const;
			tinyxml2::XMLElement* GetElement (const char*);

	};
}
