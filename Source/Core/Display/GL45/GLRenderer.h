/**
 * @file GLRenderer.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * OpenGL-specific rendering system.
 */
#pragma once

#include <string>
#include <memory>
#include <map>

#include "tinyxml2.h"
#include "Preprocess.h"
#include "GL/GL.h"

#include "AxisAlignedBox.h"

namespace Sim {

#	ifndef SIM_DOUBLE_PRECISION
		typedef GLfloat GLreal ;
#	else
		typedef GLdouble GLreal;
#	endif

	typedef struct DirLight_t {
			GLreal _direction [3];
			GLreal _ambient [3];
			GLreal _diffuse [3];
			GLreal _specular;
			GLreal _exponent;

    DirLight_t ()
    : _specular (0.), _exponent (0.)
    {
    	for (unsigned int i = 0; i < 3; ++i){
    		_direction [i] = 0.;
    	}
    	for (unsigned int i = 0; i < 3; ++i){
    		_ambient [i] = 0.;
    	}
    	for (unsigned int i = 0; i < 3; ++i){
    		_diffuse [i] = 0.;
    	}
    }
    DirLight_t (const DirLight_t& l)
    : _specular (l._specular), _exponent (l._exponent)
    {
    	memcpy (_direction, l._direction, 3*sizeof (GLreal));
    	memcpy (_ambient, l._ambient, 3*sizeof (GLreal));
    	memcpy (_diffuse, l._diffuse, 3*sizeof (GLreal));
    }
	} DirectionalLight;

	class GLProgram;
	class GLDisplayManager;

	class GLRenderer {

			friend class GLDisplayManager;

		protected:
			GLDisplayManager* _owner;
			AxisAlignedBox _bounds;
			GLreal _hither, _yon, _fov;
			GLreal _background [3]; // background color
			GLreal _projection [16], _modelview [16]; // column-major transform matrices

			// mouse-cursor attributes
			int _mouseX, _mouseY;
			unsigned int _mouseButton;

			// camera attributes
			GLreal _cameraPosition [3];
			GLreal _cameraScales [3];

			std::map <std::string, std::shared_ptr <GLProgram> > _programs;

			unsigned int _numLights;
			std::shared_ptr <DirectionalLight> _directionalLights;

		public:
			GLRenderer ();
			~GLRenderer ();

			bool Initialize (tinyxml2::XMLElement&);
			void Update ();
			void Cleanup ();

			GLuint AddProgram (const char* name, const char* location);
			GLuint GetProgramId (const char* name) const;
			bool ReloadProgram (GLuint id);

		private:
			void CheckGLVersion ();
			void SetWindowBackground (tinyxml2::XMLElement&);
			void SetCameraPosition (tinyxml2::XMLElement&);
			bool SetDirectionalLights (tinyxml2::XMLElement&);
			void EnableGLAttribs ();

		protected:
			void Mouse (unsigned int, int, int);
			void LeftMouseMotion (int, int);
			void RightMouseMotion (int, int);
			void MiddleMouseMotion (int, int);

			void UpdateProjection ();
			void UpdateModelview ();
	};
}
