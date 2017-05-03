/**
 * @file GLMesh.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See GLMesh.h.
 */

#include <climits>
#include <string>
#include <vector>
#include <list>

#include "Eigen/Core"
#include "Eigen/LU"

#include "Crc32.h"

#include "Preprocess.h"
#include "GL/GLUtils.h"
#include "Vector.h"
#include "Matrix.h"

#include "MeshLoader.h"
#include "GLMesh.h"

using std::string;
using std::vector;
using std::list;

namespace Sim {

	GLMesh::~GLMesh ()
	{
		delete [] _subset;
		_vertices.clear ();
	}

	bool GLMesh::Initialize (unsigned int depth, const char* location, const char* prefix)
	{
		for (unsigned int i = 0; i < depth; ++i){
			_nSubsets *= 8;
		}
		_subset = new Subset [_nSubsets];

		string file (location);
		if (file [file.length () - 1] != '/'){
			file += "/";
		}
		file += prefix;

		// load vertices
		string vfile (file + ".node");
		_vertices = vector <Vector> (MeshLoader::GetElementCount (vfile.c_str ()));
		if (!MeshLoader::LoadVertices <SIM_VECTOR_SIZE> (vfile.c_str (), &(_vertices [0]))){
			LOG_ERROR ("Could not load vertices from " << vfile);
			return false;
		}

		// load indices and calculate
		for (unsigned int i = 0; i < _nSubsets; ++i){
			string ifile (file + ".");
			ifile += std::to_string (i);
			ifile += ".tri";

			Subset* s = &(_subset [i]);
			s->_isize = MeshLoader::GetElementCount (ifile.c_str ());
			s->_faces = vector <unsigned int> (3*s->_isize);

			if (!MeshLoader::LoadIndices <3> (ifile.c_str (), &(s->_faces[0]))){
				LOG_ERROR ("Could not load indices from " << ifile);
				return false;
			}
		}

		// calculate the number of surface vertices
		_nSurfaceVertices = _subset [0]._faces [0];
		unsigned int offset = 0;
		for (unsigned int i = 0; i < _nSubsets; ++i){

			offset = _vertices.size ();
			Subset* s = &(_subset [i]);

			for (unsigned int j = 0; j < s->_faces.size (); ++j){
				if (_nSurfaceVertices < s->_faces [j]){
					_nSurfaceVertices = s->_faces [j];
				}
				if (offset > s->_faces [j]){
					offset = s->_faces [j];
				}
			}
			s->_voffset = offset;
		}
		++_nSurfaceVertices;

		return true;
	}

	bool GLMesh::ParametericCoordinates (unsigned int index)
	{
    // get a local subset of vertices used by current set of indices (also changes indices)
    vector <Vector> vertexSubset; // subset of vertices used by current submesh
    vector <unsigned int> newFaceIndices; // face indices mapped to the current vertex subset
    vector <unsigned int> uniqueVertIndices; // index of each subset-vertex in original vertex array
    GetVertexSubset (index, vertexSubset, newFaceIndices, uniqueVertIndices);

    // copy the newly indexed faces to internal face for the rest of the method
    // its copied back

    // get neighboring information for the new faces
    unsigned int nInternalEdges = GetTopologyInfo (index);

		return true;
	}

	void GLMesh::GetVertexSubset (unsigned int index, vector <Vector>& newVerts,
			vector <unsigned int>& newFaces, vector <unsigned int>& uniqueInds)
	{
		Subset* s = &(_subset [index]);

		// make a list of unique indices
		list <unsigned int> tmpInds;
		for (unsigned int i = 0; i < s->_faces.size (); ++i){
			tmpInds.push_back (s->_faces [i]);
		}
		tmpInds.sort ();
		tmpInds.unique ();

		uniqueInds = vector <unsigned int> (tmpInds.size ());
		for (unsigned int i = 0; i < uniqueInds.size (); ++i){
			uniqueInds [i] = tmpInds.front ();
			tmpInds.pop_front ();
		}

		// get vertex subset used by this set of indices
		newVerts = vector <Vector> (uniqueInds.size ());
		for (unsigned int i = 0; i < uniqueInds.size (); ++i){
			newVerts [i] = _vertices [uniqueInds [i]];
		}

		newFaces = vector <unsigned int> (s->_faces.size ());
		for (unsigned int i = 0; i < s->_faces.size (); ++i){

			for (unsigned int j = 0; j < uniqueInds.size (); ++j){
				if (s->_faces [i] == uniqueInds [j]){
					newFaces [i] = j;
					break;
				}
			}
		}
	}

	unsigned int GLMesh::GetTopologyInfo (unsigned int index)
	{
		Subset* s = &(_subset [index]);
		s->_neighbors = vector <Neighbor> (s->_faces.size ()/3);

    // make hash (vector) tables of size 2^16 (we use lists to deal with hash clashes
    vector <list <Edge> > edges (USHRT_MAX + 1, list <Edge> ()); // edges
    vector <list <unsigned int> > owners (USHRT_MAX + 1, list <unsigned int> ()); // edge owners
    vector <list <unsigned int> > ownerInds (USHRT_MAX + 1, list <unsigned int> ()); // edge owner index

    Crc32 crc;

    bool exists = false;
    unsigned int ind = 0;
    unsigned int inds [2];

    list <Edge>::iterator eiter;
    list <unsigned int>::iterator oiter;
    list <unsigned int>::iterator iiter;

    // iterate through face array and populate hash table
    for (unsigned int i = 0; i < s->_faces.size ()/ 3; ++i){
    	for (unsigned int j = 0; j < 3; ++j){

        // make a copy of two edges and sort them
        if (j < 2){
          inds [0] = s->_faces [3*i + j];
          inds [1] = s->_faces [3*i + j + 1];
        } else {
          inds [0] = s->_faces [3*i + 2];
          inds [1] = s->_faces [3*i];
        }
        if (inds [0] > inds [1]){
        	unsigned int tmp = inds [0];
        	inds [0] = inds [1];
        	inds [1] = tmp;
        }

        // convert the pair to string
        string str (std::to_string (inds [0]));
        str += std::to_string (inds [1]);

        // get the crc32 hash
        ind = crc.CalculateHash (str.c_str ());
        ind &= 0x0000ffff;

        // optionally insert edges into hash table
        eiter = edges [ind].begin ();
        oiter = owners [ind].begin ();
        iiter = ownerInds [ind].begin ();

        exists = false;
        while (eiter != edges [ind]. end ()){
          if (eiter->_i [0] != inds [0]){
            ++eiter;
            ++oiter;
            ++iiter;
          }
          else if (eiter->_i [1] != inds [1]){
            ++eiter;
            ++oiter;
            ++iiter;
          }
          else {

            s->_neighbors [i]._i [j] = static_cast <int> (*oiter);
            s->_neighbors [*oiter]._i [*iiter] = static_cast <int> (i);

            s->_edges.push_back (*eiter);

            edges [ind].erase (eiter);
            owners [ind].erase (oiter);
            ownerInds [ind].erase (iiter);

            exists = true;
            break;
          }
        }

        if (!exists){
          edges [ind].push_back (Edge (inds [0], inds [1]));
          owners [ind].push_back (i);
          ownerInds [ind].push_back (j);
        }

    	}
    }

    // all edges contained till now are internal edges
    unsigned int result = s->_edges.size ();

    // remaining edges are border edges
    for (unsigned int i = 0; i <= USHRT_MAX; ++i){
      while (!edges [i].empty ()){
        s->_edges.push_back (edges [i].front ());
        edges [i].pop_front ();
      }
    }

    return result;
	}

	void GLMesh::FlattenSubmesh (unsigned int index, unsigned int edgeOffset, vector <Vector>& verts, vector <unsigned int>& faces)
	{
    // calculate area-weighted average normal for the whole submesh
		// this is used to form the projection plain
    Vector normal, tmpv;
    for (unsigned int i = 0; i < faces.size (); i += 3){
      Vector e1 (verts [faces [i + 1]]); e1 -= verts [faces [i]];
      Vector e2 (verts [faces [i + 2]]); e2 -= verts [faces [i]];
      e1.Cross (tmpv, e2);
      normal += tmpv;
    }
    normal.Normalize ();

    // transform the mesh vertices to the normal plain
    Matrix3x3 transform (Matrix3x3::ZERO);
    transform (0, 0) = normal [1] * normal [1] + normal [2] * normal [2];
    transform (0, 1) = -normal [0] * normal [1];
    transform (0, 2) = -normal [0] * normal [2];
    transform (1, 0) = transform (0, 1);
    transform (1, 1) = normal [0] * normal [0] + normal [2] * normal [2];
    transform (1, 2) = -normal [1] * normal [2];
    transform (2, 0) = transform (0, 2);
    transform (2, 1) = transform (1, 2);
    transform (2, 2) = normal [0] * normal [0] + normal [1] * normal [1];

    for (unsigned int i = 0; i < verts.size (); ++i){
      verts [i] = transform * verts [i];
    }

    // rotate transformed vertices to cardinal plane closest to normal plane
    unsigned int maxi = 0;
    Real maxval = Absolute (normal [0]);
    if (Absolute (normal [1]) > maxval){
      maxi = 1;
      maxval = Absolute (normal [1]);
    }
    if (Absolute (normal [2]) > maxval){
      maxi = 2;
    }
    Vector axis (Vector::ZERO);
    if (normal [maxi] < 0.){
      axis [maxi] = -1.;
    } else {
      axis [maxi] = 1.;
    }

    Vector rotationAxis;
    normal.Cross (rotationAxis, axis);

    Real sinval = rotationAxis.Length ();
    Real cosval = static_cast <Real> (sqrt (1. - sinval * sinval));

    rotationAxis.Normalize ();

    Real u2 = rotationAxis [0]*rotationAxis [0];
    Real v2 = rotationAxis [1]*rotationAxis [1];
    Real w2 = rotationAxis [2]*rotationAxis [2];
    Real uvcos = rotationAxis [0]*rotationAxis [1]*(1. - cosval);
    Real uwcos = rotationAxis [0]*rotationAxis [2]*(1. - cosval);
    Real vwcos = rotationAxis [1]*rotationAxis [2]*(1. - cosval);
    Real usin = rotationAxis [0]*sinval;
    Real vsin = rotationAxis [1]*sinval;
    Real wsin = rotationAxis [2]*sinval;
    transform = Matrix3x3 (u2 + (1. - u2)*cosval, uvcos - wsin, uwcos + vsin,
                        uvcos + wsin, v2 + (1. - v2)*cosval, vwcos - usin,
                        uwcos - vsin, vwcos + usin, w2 + (1. - w2)*cosval);

    for (unsigned int i = 0; i < verts.size (); ++i){
      verts [i] = transform * verts [i];
    }

    // initialize temporary texture coordinates
    vector <Vector2> tmpTexCoords (verts.size ());
		switch (maxi){
		case 0:
			for (unsigned int i = 0; i < verts.size (); ++i){
				tmpTexCoords [i] = Vector2 (verts [i][1], verts [i][2]);
			}
			break;
		case 1:
			for (unsigned int i = 0; i < verts.size (); ++i){
				tmpTexCoords [i] = Vector2 (verts [i][0], verts [i][2]);
			}
			break;
		case 2:
			for (unsigned int i = 0; i < verts.size (); ++i){
				tmpTexCoords [i] = Vector2 (verts [i][0], verts [i][1]);
			}
		}

    // tag border vertices as such and make a count
		Subset* s = &(_subset [index]);
    vector <bool> borderFlag (verts.size (), false);
    for (unsigned int i = edgeOffset; i < s->_edges.size (); ++i){

    }

	}
}
