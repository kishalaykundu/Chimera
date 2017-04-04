/**
 * @file CuglMsdRender.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See CuglMsdRender.h.
 */

#include <cstring>
#include "tinyxml2.h"

#include "Preprocess.h"

#include "MeshLoader.h"
#include "PNGUtils.h"
#include "GL/GLUtils.h"

#include "Driver.h"
#include "Assets/Geometry.h"
#include "CuglMsdRender.h"

using tinyxml2::XMLElement;
using tinyxml2::XMLError;

namespace Sim {
	namespace Assets {

		CuglMsdRender::CuglMsdRender ()
		: _vertexArray (0), _positionBuffer (0), _colorTexture (0), _indexBuffer (0),
			_normalFramebuffer (0), _normalTexture (0), _colorTextureExists (false)
		{
			for (unsigned int i = 0; i < 2; ++i){
				_texCoordBuffers [i] = 0;
			}
			for (unsigned int i = 0; i < 2; ++i){
				_normalFramebufferDimensions [i] = 0;
			}
			for (unsigned int i = 0; i < 2; ++i){
				_program [i] = 0;
			}
		}

		CuglMsdRender::~CuglMsdRender ()
		{

		}

		// initialization method
		bool CuglMsdRender::Initialize (XMLElement& config, Asset* asset)
		{
			// load the programs to be used for rendering purposes
			XMLElement* element = config.FirstChildElement ("Programs");
			if (element != nullptr){
				LOG_ERROR ("No shading programs specified in " << config.Value ());
				return false;
			}
			if (!LoadPrograms (*element)){
				LOG_ERROR ("Could not load all shader programs in " << config.Value ());
				return false;
			}
			element = nullptr;

			// initialize and bind vertex array so buffers can be added
			glCreateVertexArrays (1, &_vertexArray); LOG_GL_ERROR ();
			glBindVertexArray (_vertexArray); LOG_GL_ERROR ();

			// initialize position buffer - this will be populated by the CUDA physics component
			glCreateBuffers (1, &_positionBuffer); LOG_GL_ERROR ();
			glBindBuffer (GL_ARRAY_BUFFER, _positionBuffer); LOG_GL_ERROR ();

			// initialize normal texture coordinates
			glGenBuffers (2, _texCoordBuffers); LOG_GL_ERROR ();
			glBindBuffer (GL_ARRAY_BUFFER, _texCoordBuffers [0]); LOG_GL_ERROR ();

			element = config.FirstChildElement ("NormalTexture");
			if (element == nullptr){
				LOG_ERROR ("No normal texture coordinates specified");
				return false;
			}
			const char* location = element->Attribute ("Location");
			if (location == nullptr){
				LOG_ERROR ("No file specified for normal texture coordinates");
				return false;
			}
			Geometry* g = asset->GetComponent <Geometry> ("Geometry").get ();
			Vector2* ntc = new Vector2 [g->SurfaceVertexCount()];
			if (!MeshLoader::LoadVertices <2> (location, ntc)){
				LOG_ERROR ("Could not load normal texture coordinates from " << location);
				return false;
			}
			element = nullptr;

			glNamedBufferData (_texCoordBuffers [0], sizeof (Vector2)*g->SurfaceVertexCount (),
					&ntc, GL_STATIC_DRAW); LOG_GL_ERROR ();
			glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, 0); LOG_GL_ERROR ();
			glEnableVertexAttribArray (1); LOG_GL_ERROR ();
			delete [] ntc;

			// check if asset has texture and load it if all information is given
			element = config.FirstChildElement ("ColorTexture");
			if (element != nullptr){

				XMLElement* ctelem = element->FirstChildElement ("TextureCoordinates");
				if (ctelem == nullptr){
					LOG_WARNING ("No texture coordinate file specified for color texture");
					glDeleteBuffers (1, &(_texCoordBuffers [1])); LOG_GL_ERROR ();
				}
				else {
					Vector2* ctc = new Vector2 [g->SurfaceVertexCount ()];
					if (!MeshLoader::LoadVertices <2> (ctelem->Value (), ctc)){
						LOG_WARNING ("Could not load color texture coordinates from " << ctelem->Value ());
					}
					else {
						glBindBuffer (GL_ARRAY_BUFFER, _texCoordBuffers [1]); LOG_GL_ERROR ();
						glNamedBufferDataEXT (_texCoordBuffers [1], sizeof (Vector2)*g->SurfaceVertexCount (),
								&ctc, GL_STATIC_DRAW); LOG_GL_ERROR ();
						glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 0, 0); LOG_GL_ERROR ();
						glEnableVertexAttribArray (2); LOG_GL_ERROR ();
					}
				}

				XMLElement* loc = element->FirstChildElement ("Location");

				if (loc == nullptr){
					LOG_WARNING ("No location specified for color texture...ignoring texture");
				}
				else {
					XMLError error;
					int width = 0, height = 0;

					bool legitFlag = true;
					if ((error = element->QueryIntAttribute ("Width", &width)) != tinyxml2::XML_SUCCESS){
						LOG_WARNING ("No width specified for texture " << loc->Value () << "...ignoring texture");
						legitFlag = false;
					}
					if ((error = element->QueryIntAttribute ("Height", &height)) != tinyxml2::XML_SUCCESS){
						LOG_WARNING ("No height specified for texture " << loc->Value () << "...ignoring texture");
						legitFlag = false;
					}

					_colorTextureExists = legitFlag;
					if (legitFlag) {
						unsigned char* image;
						if (!DecodePNG (loc->Value (), width, height, image)){
							LOG_WARNING ("Could not read texture image file " << loc->Value ());
						}
						else {
							glGenTextures (1, &_colorTexture); LOG_GL_ERROR ();
							glBindTexture (GL_TEXTURE_2D, _colorTexture); LOG_GL_ERROR ();
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); LOG_GL_ERROR ();
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); LOG_GL_ERROR ();
							float color[] = { 0., 0., 0., 0 };
							glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color); LOG_GL_ERROR ();
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); LOG_GL_ERROR ();
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); LOG_GL_ERROR ();
							glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); LOG_GL_ERROR ();
							delete [] image;
							glBindTexture (GL_TEXTURE_2D, 0);
						}
					}

				}
			}

			// let go of the vertex attribute elements
			glBindBuffer (GL_ARRAY_BUFFER, 0);
			glBindVertexArray (0);

			// initialize index buffer
			glGenBuffers (1, &_indexBuffer); LOG_GL_ERROR ();
			glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, _indexBuffer); LOG_GL_ERROR ();
			unsigned int* iptr = g->FaceIndexBuffer ();
			glNamedBufferDataEXT (_indexBuffer, 3*sizeof (unsigned int)*g->FaceIndexCount (),
					&iptr, GL_STATIC_DRAW); LOG_GL_ERROR ();
			glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);

			glGenBuffers (1, &_normalFramebuffer); LOG_GL_ERROR ();
			glGenBuffers (1, &_normalTexture); LOG_GL_ERROR ();

			return true;
		}

		// cleanup
		void CuglMsdRender::Cleanup ()
		{

		}

		// program loader method
		bool CuglMsdRender::LoadPrograms (XMLElement& element)
		{
			XMLElement* plist = element.FirstChildElement ("Program");
			if (plist == nullptr){
				LOG_ERROR ("No programs specified in config");
				return false;
			}

			while (plist != nullptr){

				const char* prefix = plist->Attribute ("Prefix");
				if (prefix == nullptr){
					LOG_ERROR ("No shader program prefix specified");
					return false;
				}

				const char* location = plist->Attribute ("Location");
				if (location == nullptr){
					LOG_ERROR ("No shader program location specified");
					return false;
				}

				if (!strcmp ("Normal", plist->Attribute ("Type"))){
					_program [0] = Driver::Instance ().AddGPUProgram (prefix, location);
				}
				else if (!strcmp ("Render", plist->Attribute ("Type"))){
					_program [1] = Driver::Instance ().AddGPUProgram (prefix, location);
				}
				else {
					LOG_ERROR ("Unrecognized shader program " << plist->Attribute ("Type"));
				}

				plist = plist->NextSiblingElement ("Program");
			}
			if (!_program [0] || !_program [1]){
				LOG_ERROR ("All needed programs not loaded");
				return false;
			}
			return true;
		}

		// method to initialize vertex buffer objects and related variables
		bool CuglMsdRender::InitializeVBOs (XMLElement& element, Asset* a)
		{
			return true;
		}

	}
}
