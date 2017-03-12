/**
 * @file Geometry.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See Geometry.h.
 */

#include <cstdio>
#include <string>
#include <memory>

#include "tinyxml2.h"
#include "Preprocess.h"

#include "InputParser.h"
#include "MeshLoader.h"
#include "Assets/Geometry.h"

using std::string;
using std::shared_ptr;
using std::make_shared;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;
using tinyxml2::XMLError;

namespace Sim {
	namespace Assets {

		Geometry::Geometry ()
		: _numVertices (0), _numSurfaceVertices (0), _numSubsets (1)
		{}

		bool Geometry::Initialize (const char* config)
		{
			InputParser ip;
			if (!ip.Initialize (config, "MeshGeometry")){
				LOG_ERROR ("Could not open " << config << " with required root \'MeshGeometry\'");
				return false;
			}

			// get asset name
			const XMLElement* elem = ip.GetElement ("Name");
			if (elem == nullptr){
				LOG_ERROR ("No asset geometry \'Name\' specified (mandatory)");
				return false;
			}
			const char* name = elem->Value ();

			// get asset location
			elem = ip.GetElement ("Location");
			if (elem == nullptr){
				LOG_ERROR ("No asset geometry \'Location\' specified (mandatory)");
				return false;
			}
			const char* location = elem->Value ();

			unsigned int depth = 0;
			elem = ip.GetElement ("Depth");
			XMLError error;
			if ((error = elem->QueryUnsignedText(&depth)) != tinyxml2::XML_SUCCESS){
				LOG_ERROR ("Error reading partition depth from " << config);
				return false;
			}
			for (unsigned int i = 0; i < depth; ++i){
				_numSubsets *= 8;
			}

			// make a prefix from location and name (to be used subsequently to read files later)
			string prefix (location);
			if (prefix [prefix.size () - 1] != '/'){
				prefix += "/";
			}
			prefix += std::to_string (depth);
			prefix += "/";
			prefix += name;

			string file (prefix);
			file += ".node";

			if (!ReadVertexFile (file.c_str ())){
				LOG_ERROR ("Failed to read vertex file for Geometry component of " << name);
				Cleanup ();
				return false;
			}

			if (!ReadIndexFiles (prefix.c_str ())){
				LOG_ERROR ("Failed to read index files for Geometry component of " << name);
				Cleanup ();
				return false;
			}
			UpdateSurfaceVertexCount ();

			return true;
		}

		void Geometry::Update ()
		{
			_current.swap (_previous);
			for (unsigned int i = 0; i < _numSubsets; ++i){
				_subsets.get () [i].UpdateBound (_current);
			}
		}

		void Geometry::Cleanup ()
		{
			_current.reset ();
			_previous.reset ();
			_subsets.reset ();
		}

		bool Geometry::ReadVertexFile (const char* file)
		{
			if (!MeshLoader::LoadVertices (file, _numVertices, _current, _previous)){
				LOG_ERROR ("Could not read vertex file " << file);
				return false;
			}

			// update axis-aligned bounding box
			Vector min (_current.get () [0]);
			Vector max (min);
			for (unsigned int i = 1; i < _numVertices; ++i){
				for (unsigned int j = 0; j < 3; ++j){
					if (min [j] > _current.get () [i][j]){
						min [j] = _current.get () [i][j];
					}
				}
				for (unsigned int j = 0; j < 3; ++j){
					if (max [j] < _current.get () [i][j]){
						max [j] = _current.get () [i][j];
					}
				}
			}
			_bounds.Update (min, max);
			return true;
		}

		bool Geometry::ReadIndexFiles (const char* prefix)
		{
			_subsets = shared_ptr <SpatialSubset> (new SpatialSubset [_numSubsets], DeleteArray <SpatialSubset> ());

			for (unsigned int i = 0; i < _numSubsets; ++i){
				string file (prefix);
				file += ".";
				file += std::to_string (i);
				file += ".tri";

				SpatialSubset* s = &(_subsets.get () [i]);
				if (!MeshLoader::LoadIndices (file.c_str (), 3, s->_numFaces, s->_faces)){
					LOG_ERROR ("Could not load index file " <<  file);
					return false;
				}

				s->UpdateBound (_current);
			}
			return true;
		}

		void Geometry::UpdateSurfaceVertexCount ()
		{
			for (unsigned int i = 0; i < _numSubsets; ++i){
				for (unsigned int j = 0; j < 3*_subsets.get () [i]._numFaces; ++j){
					if (_numSurfaceVertices < _subsets.get () [i]._faces.get () [j]){
						_numSurfaceVertices = _subsets.get () [i]._faces.get () [j];
					}
				}
			}
			// this is done because at this point _numSurfaceVertices
			// has the index of the last surface vertex
			++_numSurfaceVertices;
		}
	}
}
