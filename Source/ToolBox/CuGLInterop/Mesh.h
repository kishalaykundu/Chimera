/**
 * @file Mesh.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * Mesh geometry.
 */
#pragma once

#include <memory>
#include <mutex>

namespace Sim {

	class Vector;

	class Mesh {

	protected:
		std::mutex _mutex;

		unsigned int _numVertices;
		std::shared_ptr <Vector> _current;
		std::shared_ptr <Vector> _previous;

		unsigned int _numFaces;
		std::shared_ptr <unsigned int> _faces;

	public:
		bool Initialize (const char* vertexFile, const char* indexFile);
		void Cleanup ();
	};
}
