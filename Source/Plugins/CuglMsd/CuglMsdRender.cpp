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
#include "GL/GLUtils.h"

#include "Driver.h"
#include "CuglMsdRender.h"

using tinyxml2::XMLElement;
using tinyxml2::XMLError;

namespace Sim {
	namespace Assets {

		CuglMsdRender::CuglMsdRender ()
		: _vertexArray (0), _colorTexture (0), _indexBuffer (0),
			_normalFramebuffer (0), _normalTexture (0), _colorTextureExists (false)
		{
			for (unsigned int i = 0; i < 3; ++i){
				_bufferObjects [i] = 0;
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

			// check if asset has texture and load it if all information is given
			element = config.FirstChildElement ("ColorTexture");
			if (element != nullptr){

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
						glGenBuffers (1, &_colorTexture); LOG_GL_ERROR ();
					}

				}
			}

			// initialize (but not populate) buffer objects
			glGenVertexArrays (1, &_vertexArray); LOG_GL_ERROR ();
			glGenBuffers (3, _bufferObjects); LOG_GL_ERROR ();
			glGenBuffers (1, &_indexBuffer); LOG_GL_ERROR ();
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
	}
}
