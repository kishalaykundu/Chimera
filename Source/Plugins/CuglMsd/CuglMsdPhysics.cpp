/**
 * @file CuglMsdPhysics.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See CuglMsdPhysics.h.
 */
#include <memory>

#include "tinyxml2.h"

#include "Config.h"
#include "CUDA/CUDAUtils.h"

#include "Assets/Asset.h"
#include "CuglMsdPhysics.h"
#include "CuglMsdRender.h"

using std::shared_ptr;
using std::static_pointer_cast;
using tinyxml2::XMLElement;

namespace Sim {
	namespace Assets {

		CuglMsdPhysics::CuglMsdPhysics ()
		: _vertices (nullptr), _indices (nullptr)
		{}

		CuglMsdPhysics::~CuglMsdPhysics ()
		{

		}

		bool CuglMsdPhysics::Initialize (XMLElement& config, Asset* asset)
		{
			CuglMsdRender* c = asset->GetComponent <CuglMsdRender> ("Render").get ();

			LOG_CUDA_RESULT (cuGraphicsGLRegisterBuffer (_vertices, c->_bufferObjects [0], CU_GRAPHICS_REGISTER_FLAGS_NONE));

			// load spring indices from file


			return true;
		}

		void CuglMsdPhysics::Cleanup ()
		{

		}
	}
}
