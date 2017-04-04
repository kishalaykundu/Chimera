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
#include "Vector.h"
#include "Assets/Geometry.h"

using std::string;
using std::shared_ptr;
using std::make_shared;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;
using tinyxml2::XMLError;

using Sim::Vector;

namespace Sim {
	namespace Assets {

		Geometry::Geometry ()
		: _offsetIndex (1), _offsetSize (0), _numVertices (0),
			_numSurfaceVertices (0), _numFaces (0), _numSubsets (1)
		{}

		bool Geometry::Initialize (XMLElement& config, Asset* asset)
		{
			// get asset name prefix
			const char* name = config.Attribute ("Prefix");
			if (name == nullptr){
				LOG_ERROR ("No asset geometry \'Prefix\' specified (mandatory)");
				return false;
			}

			// get asset location
			const char* location = config.Attribute ("Location");
			if (location == nullptr){
				LOG_ERROR ("No asset geometry \'Location\' specified (mandatory)");
				return false;
			}

			unsigned int depth = 0;
			XMLError error;
			if ((error = config.QueryUnsignedAttribute("Depth", &depth)) != tinyxml2::XML_SUCCESS){
				LOG_ERROR ("Could not read partition depth");
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
			_offsetSize = SIM_VECTOR_SIZE * sizeof (Vector) * _numVertices;

			return true;
		}

		void Geometry::Update ()
		{
			++_offsetIndex;
		}

		void Geometry::Cleanup ()
		{
			_vertices.reset ();
			_faces.reset ();
			_subsets.reset ();
		}

		bool Geometry::ReadVertexFile (const char* file)
		{
			_numVertices = MeshLoader::GetElementCount (file);
			_vertices = std::shared_ptr <Vector> (new Vector [3*_numVertices], DeleteArray <Vector> ());
			if (!_vertices){
				LOG_ERROR ("Could not allocate 1st vertex array of size " << _numVertices << " from " << file);
				return false;
			}

			if (!MeshLoader::LoadVertices <SIM_VECTOR_SIZE> (file, _vertices.get ())){
				LOG_ERROR ("Could not read vertex file " << file);
				return false;
			}
			// populate 2nd buffer
			unsigned int offset = _numVertices;
			Vector* vptr = _vertices.get ();
			for (unsigned int i = 0; i < _numVertices; ++i){
				vptr [offset + i] = vptr [i];
			}
			// populate 3rd buffer
			offset *= 2;
			for (unsigned int i = 0; i < _numVertices; ++i){
				vptr [offset + i] = vptr [i];
			}

			// update axis-aligned bounding box
			Vector min (vptr [0]);
			Vector max (min);
			for (unsigned int i = 1; i < _numVertices; ++i){
				for (unsigned int j = 0; j < 3; ++j){
					if (min [j] > vptr [i][j]){
						min [j] = vptr [i][j];
					}
				}
				for (unsigned int j = 0; j < 3; ++j){
					if (max [j] < vptr [i][j]){
						max [j] = vptr [i][j];
					}
				}
			}
			_bounds.Update (min, max);
			return true;
		}

		bool Geometry::ReadIndexFiles (const char* prefix)
		{
			_subsets = shared_ptr <SpatialSubset> (new SpatialSubset [_numSubsets], DeleteArray <SpatialSubset> ());

			// get total number of faces
			for (unsigned int i = 0; i < _numSubsets; ++i){
				string file (prefix);
				file += ".";
				file += std::to_string (i);
				file += ".tri";

				SpatialSubset* s = &(_subsets.get () [i]);
				s->_isize = MeshLoader::GetElementCount (file.c_str ());
				s->_ioffset = 3*_numFaces;
				_numFaces += s->_isize;
			}

			// initialize face index array
			_faces = shared_ptr <unsigned int> (new unsigned int [3*_numFaces], DeleteArray <unsigned int> ());

			for (unsigned int i = 0; i < _numSubsets; ++i){
				string file (prefix);
				file += ".";
				file += std::to_string (i);
				file += ".tri";

				SpatialSubset* s = &(_subsets.get () [i]);
				unsigned int* f = &(_faces.get () [s->_ioffset]);
				if (!MeshLoader::LoadIndices <3> (file.c_str (), f)){
					LOG_ERROR ("Could not load index file " <<  file);
					return false;
				}

				// calculate the offset of vertex
				unsigned int min = f [0];
				for (unsigned int j = 1; j < 3*s->_isize; ++j){
					if (min > f [j]){
						min = f [j];
					}
				}
				s->_voffset = min;

				s->UpdateBound (CurrentVertexBuffer (), f);
			}
			return true;
		}

		void Geometry::UpdateSurfaceVertexCount ()
		{
			unsigned int* f = _faces.get ();
			for (unsigned int i = 0; i < 3*_numFaces; ++i){
				if (_numSurfaceVertices < f [i]){
					_numSurfaceVertices = f [i];
				}
			}
			// this is done because at this point _numSurfaceVertices
			// has the index of the last surface vertex
			++_numSurfaceVertices;
		}
	}
}
