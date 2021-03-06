/**
 * @file CuglMsdRender.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The graphics component for the CuglMsd data type. It is
 * extended from Render component type.
 */
#pragma once

#include "tinyxml2.h"

#include "GL/GLUtils.h"
#include "Assets/Render.h"

namespace Sim {

	class Asset;

	namespace Assets {

		class CuglMsdRender : public Render {

			friend class CuglMsdPhysics;

		protected:
			GLuint _vertexArray;

			GLuint _positionBuffer; // vertex positions (3*number of vertices)
			/**
			 * Buffer objects specifications:
			 * 1. Normal texture coordinates
			 * 2. Color texture coordinates
			 */
			GLuint _texCoordBuffers [2];
			GLuint _colorTexture;
			GLuint _indexBuffer;

			unsigned int _normalFramebufferDimensions [2];
			GLuint _normalFramebuffer;
			GLuint _normalTexture;

			/**
			 * Program ID specifications:
			 * 1. Normal calculation program
			 * 2. Rendering program
			 */
			GLuint _program [2];

			bool _colorTextureExists;

		public:
			CuglMsdRender ();
			~CuglMsdRender ();

			virtual bool Initialize (tinyxml2::XMLElement& config, Asset* asset) override;
			virtual void Cleanup () override;

		protected:
			bool LoadPrograms (tinyxml2::XMLElement&);
			bool InitializeVBOs (tinyxml2::XMLElement&, Asset*);
		};
	}
}
