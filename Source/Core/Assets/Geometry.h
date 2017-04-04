/**
 * @file Geometry.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The geometry component interface for the Asset class in the Chimera
 * class. It is derived from the generic Component interface. Geometry
 * loads all the vertices and face-indices for any asset.
 */
#pragma once

#include <string>
#include <memory>

#include "Preprocess.h"
#include "Vector.h"
#include "AxisAlignedBox.h"
#include "Assets/Component.h"

namespace Sim {
	namespace Assets {

		class Geometry : public Component {

			private:
				class SpatialSubset {
				public:
					unsigned int _voffset;
					unsigned int _ioffset;
					unsigned int _isize;
					AxisAlignedBox _bound;

					SpatialSubset (): _voffset (0), _ioffset (0), _isize (0) {}
					~SpatialSubset () {}

					// udpate the axis-aligned bounding box for subset
					void UpdateBound (Vector* vertices, unsigned int* faces)
					{
						Vector min (vertices [faces [0] - _voffset]);
						Vector max (min);
						for (unsigned int i = 1; i < 3*_isize; ++i){
							for (unsigned int j = 0; j < 3; ++j){
								if (min [j] > vertices [faces [i] - _voffset][j]){
									min [j] = vertices [faces [i] - _voffset][j];
								}
							}
							for (unsigned int j = 0; j < 3; ++j){
								if (max [j] < vertices [faces [i] - _voffset][j]){
									max [j] = vertices [faces [i] - _voffset][j];
								}
							}
						}
						_bound.Update (min, max);
					}
				};

			protected:
				AxisAlignedBox _bounds;

				unsigned int _offsetIndex;
				unsigned int _offsetSize;
	      unsigned int _numVertices;
	      unsigned int _numSurfaceVertices;
	      std::shared_ptr <Vector> _vertices;

				unsigned int _numFaces;
				std::shared_ptr <unsigned int> _faces;

	      unsigned int _numSubsets;
	      std::shared_ptr <SpatialSubset> _subsets;

			public:
				Geometry ();
				virtual ~Geometry () {Cleanup ();}

				virtual const std::string Name () const final {return "Geometry";}

				virtual bool Initialize (tinyxml2::XMLElement& config, Asset* asset) override;
				virtual void Update () override;
				virtual void Cleanup () override;

				unsigned int VertexCount () const {return _numVertices;}
				unsigned int SurfaceVertexCount () const {return _numSurfaceVertices;}
				Vector* PreviousVertexBuffer () {return &(_vertices.get () [(_offsetIndex - 1)*_numVertices]);}
				Vector* CurrentVertexBuffer () {return &(_vertices.get () [_offsetIndex*_numVertices]);}

				unsigned int FaceIndexCount () const {return _numFaces;}
				unsigned int* FaceIndexBuffer () {return _faces.get ();}
				unsigned int* FaceIndexBuffer (unsigned int index)
				{
#					ifndef NDEBUG
					if (index >= _numSubsets){
						LOG_ERROR ("Accessing invalid index buffer no" << index << " (count: " << _numSubsets << ")");
						return nullptr;
					} else {
#					endif
						return &(_faces.get () [_subsets.get () [index]._ioffset]);
#					ifndef NDEBUG
					}
#					endif
				}

			protected:
				bool ReadVertexFile (const char* file);
				bool ReadIndexFiles (const char* file);
				void UpdateSurfaceVertexCount ();
		};
	}
}
