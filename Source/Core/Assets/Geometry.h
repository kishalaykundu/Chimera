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
						AxisAlignedBox _bound;
						unsigned int _numFaces;
						std::shared_ptr <unsigned int> _faces;

						SpatialSubset (): _numFaces (0) {}
						~SpatialSubset () {_faces.reset ();}

						// udpate the axis-aligned bounding box for subset
						void UpdateBound (std::shared_ptr <Vector> vertices)
						{
							Vector min (vertices.get () [_faces.get () [0]]);
							Vector max (min);
							for (unsigned int i = 1; i < 3*_numFaces; ++i){
								for (unsigned int j = 0; j < 3; ++j){
									if (min [j] > vertices.get () [_faces.get () [i]][j]){
										min [j] = vertices.get () [_faces.get () [i]][j];
									}
								}
								for (unsigned int j = 0; j < 3; ++j){
									if (max [j] < vertices.get () [_faces.get () [i]][j]){
										max [j] = vertices.get () [_faces.get () [i]][j];
									}
								}
							}
							_bound.Update (min, max);
						}
				};

			protected:
				AxisAlignedBox _bounds;

	      unsigned int _numVertices;
	      unsigned int _numSurfaceVertices;
	      std::shared_ptr <Vector> _current;
	      std::shared_ptr <Vector> _previous;

	      unsigned int _numSubsets;
	      std::shared_ptr <SpatialSubset> _subsets;

			public:
				Geometry ();
				virtual ~Geometry () {Cleanup ();}

				virtual const std::string Name () const final {return "Geometry";}

				virtual bool Initialize (const char*) override;
				virtual void Update () override;
				virtual void Cleanup () override;

				unsigned int VertexCount () const {return _numVertices;}
				unsigned int SurfaceVertexCount () const {return _numSurfaceVertices;}
				std::shared_ptr <Vector> CurrentVertexBuffer () {return _current;}
				std::shared_ptr <Vector> PreviousVertexBuffer () {return _previous;}

				unsigned int FaceIndexSubsetCount () const {return _numSubsets;}
				std::shared_ptr <unsigned int> FaceIndexBuffer (unsigned int index)
				{
#					ifndef NDEBUG
					if (index >= _numSubsets){
						LOG_ERROR ("Accessing invalid index buffer no" << index << " (count: " << _numSubsets << ")");
						std::shared_ptr <unsigned int> p;
						return p;
					} else {
#					endif
						return _subsets.get () [index]._faces;
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
