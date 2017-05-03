/**
 * @file GLMesh.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * OpenGL-based mesh used to generate texture atlas.
 */
#pragma once

#include <vector>

#include "Vector.h"
#include "GL/GLUtils.h"

namespace Sim {

	class GLMesh {

	private:
		class Edge {

		public:
			unsigned int _i [2];

			inline Edge () {_i [0] = 0; _i [1] = 0;}
			inline Edge (unsigned int i1, unsigned int i2) {_i [0] = i1; _i [1] = i2;}
			inline Edge (const Edge& e) {_i [0] = e._i [0]; _i [1] = e._i [1];}
			inline Edge& operator = (const Edge& e) {_i [0] = e._i [0]; _i [1] = e._i [1]; return *this;}
		};

		class Neighbor {
		public:
			int _i [3];

			inline Neighbor ()
			{
				for (unsigned int i = 0; i < 3; ++i){
					_i [i] = -1;
				}
			}
		};

		class Subset {

		public:
			unsigned int _voffset;
			unsigned int _ioffset;
			unsigned int _isize;
			std::vector <unsigned int> _faces;
			std::vector <Edge> _edges;
			std::vector <Neighbor> _neighbors;

			Subset ()
			: _voffset (0), _ioffset (0), _isize (0),
			_faces (0), _edges (0), _neighbors (0)
			{}
			~Subset ()
			{
				_faces.clear ();
			}
		};
	protected:
		unsigned int _nSurfaceVertices = 0;
		std::vector <Vector> _vertices;

		std::vector <Vector3> _texCoords3D;
		std::vector <Vector2> _texCoords2D;

		unsigned int _nSubsets = 1;
		Subset* _subset = nullptr;

	public:
		GLMesh () = default;
		~GLMesh ();

		bool Initialize (unsigned int depth, const char* location, const char* prefix);
		bool ParametericCoordinates (unsigned int index);

	protected:
		void GetVertexSubset (unsigned int index, std::vector <Vector>& newVerts,
				std::vector <unsigned int>& newFaces, std::vector <unsigned int>& uniqueInds);
		unsigned int GetTopologyInfo (unsigned int index);
		void FlattenSubmesh (unsigned int index, unsigned int edgeOffset, std::vector <Vector>& verts, std::vector <unsigned int>& faces);
	};
}
