/**
 * @file Mesh.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * Mesh geometry.
 */

#include <memory>

#include "Preprocess.h"

#include "MeshLoader.h"
#include "Vector.h"
#include "Mesh.h"

namespace Sim {

	Mesh::Mesh ()
	: _numVertices (0), _numFaces (0)
	{ }

	Mesh::~Mesh ()
	{
		Cleanup ();
	}

	bool Mesh::Initialize (const char* vf, const char* ff)
	{
		if (!MeshLoader::LoadVertices (vf, _numVertices, _current, _previous)){
			Cleanup ();
			return false;
		}
		if (!MeshLoader::LoadIndices (ff, 3, _numFaces, _faces)){
			Cleanup ();
			return false;
		}
		return true;
	}

	void Mesh::Cleanup ()
	{
		_current.clear ();
		_previous.clear ();
		_faces.clear ();
	}
}
