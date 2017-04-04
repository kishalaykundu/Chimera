/**
 * @file InputParser.cpp
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
 * See InputParser.h
 */

#include <cstring>

#include "tinyxml2.h"
#include "Preprocess.h"
#include "InputParser.h"

using tinyxml2::XMLElement;

namespace Sim {

	bool InputParser::Initialize (const char* config, const char* rootName)
	{
		_doc.LoadFile (config);
#ifndef NDEBUG
		if (_doc.Error ()){
			LOG_ERROR ("Could not load " << config << ": " << _doc.ErrorName ());
			return false;
		}
#endif

		_root = _doc.FirstChild ();
#ifndef NDEBUG
		if (!_root){
			LOG_ERROR ("Could not grab root element from " << config << ": " << _doc.ErrorName () << "...Exiting");
			return false;
		}
		if (strcmp (_root->Value (), rootName)){
			LOG_ERROR ("Root element in " << config << " is " << _root->Value () << " (not \'" << rootName << "\')...Exiting");
			return false;
		}
#endif
		return true;
	}

	const char* InputParser::DocName () const
	{
		return _doc.Value ();
	}

	XMLElement* InputParser::GetElement (const char* name)
	{
		XMLElement* result = _root->FirstChildElement (name);
		if (result == nullptr){
			LOG_ERROR ("Could not find " << name << " in " << _root->Value ());
		}
		return result;
	}
}
