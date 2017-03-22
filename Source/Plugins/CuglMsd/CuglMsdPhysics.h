/**
 * @file CuglMsdPhysics.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The CUDA based physics component for the CuglMsd data type. It is
 * extended from Physics component type.
 */
#pragma once

#include "tinyxml2.h"

#include "CUDA/CUDAUtils.h"
#include "Assets/Physics.h"

namespace Sim {

	class Asset;

	namespace Assets {

		class CuglMsdPhysics : public Physics {

		protected:
			CUgraphicsResource* _vertices;
			CUdeviceptr* _indices;

		public:
			CuglMsdPhysics ();
			~CuglMsdPhysics ();

			virtual bool Initialize (tinyxml2::XMLElement& config, Asset* asset) override;
			virtual void Cleanup () override;

		};
	}
}
