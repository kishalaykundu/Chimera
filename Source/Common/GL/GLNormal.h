/**
 * @file GLNormal.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * OpenGL-related class containing all variables related to
 * GPU-based normal calculating program.
 */
#pragma once

#include <memory>

#include "Preprocess.h"
#include "GL/GL.h"

namespace Sim {

	class GLNormal {

		private:
			class SpatialSubset {
				public:
					unsigned int _numFaces;
					// element array to hold the outside indices for normal calculation
					GLuint _indexBufferId;

					SpatialSubset (): _numFaces (0), _indexBufferId (0) {}
					~SpatialSubset () {}
			};

		protected:
      GLuint _framebufferDimensions [2]; // normal framebuffer dimensions
      GLuint _framebufferId; // on-device framebuffer holding render texture
      GLuint _texCoordBufferId; // on-device texture coordinate buffer
      GLuint _textureId; // on-device 2D render texture for storing normals
      GLuint _vertexArrayId [2]; // vertex array to hold the buffers

      GLint _programTextureLocation; // GLSL-program texture location identifier
      GLuint _programId;

      unsigned int _numSubsets;
      std::shared_ptr <SpatialSubset> _subsets;

		public:
      GLNormal ()
			: _framebufferId (0), _texCoordBufferId (0), _textureId (0),
				_programTextureLocation (0), _programId (0), _numSubsets (0)
			{
      	_framebufferDimensions [0] = 0; _framebufferDimensions [1] = 0;
      	_vertexArrayId [0] = 0; _vertexArrayId [1] = 0;
			}
      ~GLNormal () {_subsets.reset ();}

      // accessor and mutator methods
      GLuint FrameBufferDimension (int index) const
      {
#				ifndef NDEBUG
      	if (index > 1 || index < 0){
      			LOG_WARNING ("Framebuffer dimension index " << index << " is out of bounds (must be 0 or 1)...returning index 0");
      			return _framebufferDimensions [0];
      	}
#				endif
      	return _framebufferDimensions [index];
      }
      GLuint& FrameBufferDimension (int index)
      {
#				ifndef NDEBUG
      	if (index > 1 || index < 0){
      			LOG_WARNING ("Framebuffer dimension index " << index << " is out of bounds (must be 0 or 1)...returning index 0");
      			return _framebufferDimensions [0];
      	}
#				endif
      	return _framebufferDimensions [index];
      }

      GLuint FramebufferId () const {return _framebufferId;}
      GLuint& FramebufferId () {return _framebufferId;}

      GLuint TextureCoordinateBufferId () const {return _texCoordBufferId;}
      GLuint& TextureCoordinateBufferId () {return _texCoordBufferId;}

      GLuint TextureId () const {return _textureId;}
      GLuint& TextureId () {return _textureId;}

      GLuint VertexArrayId (int index) const
      {
#				ifndef NDEBUG
      	if (index > 1 || index < 0){
      			LOG_WARNING ("Vertex array Id index " << index << " is out of bounds (must be 0 or 1)...returning index 0");
      			return _vertexArrayId [0];
      	}
#				endif
      	return _vertexArrayId [index];
      }
      GLuint& VertexArrayId (int index)
      {
#				ifndef NDEBUG
      	if (index > 1 || index < 0){
      			LOG_WARNING ("Vertex array Id index " << index << " is out of bounds (must be 0 or 1)...returning index 0");
      			return _vertexArrayId [0];
      	}
#				endif
      	return _vertexArrayId [index];
      }

      GLint ProgramTextureLocation () const {return _programTextureLocation;}
      GLint& ProgramTextureLocation () {return _programTextureLocation;}

      GLuint ProgramId () const {return _programId;}
      GLuint& ProgramId () {return _programId;}

      GLuint IndexBufferId (unsigned int index) const
      {
#				ifndef NDEBUG
				if (index >= _numSubsets){
					LOG_ERROR ("Accessing invalid index buffer no" << index << " (count: " << _numSubsets << ")");
					return 0;
				} else {
#				endif
				return _subsets.get () [index]._indexBufferId;
#				ifndef NDEBUG
				}
#				endif
      }
      GLuint& IndexBufferId (unsigned int index)
      {
#				ifndef NDEBUG
				if (index >= _numSubsets){
					LOG_ERROR ("Accessing invalid index buffer no" << index << " (count: " << _numSubsets << ")...returning index [0]");
					return _subsets.get () [0]._indexBufferId;
				} else {
#				endif
				return _subsets.get () [index]._indexBufferId;
#				ifndef NDEBUG
				}
#				endif
      }
	};
}
