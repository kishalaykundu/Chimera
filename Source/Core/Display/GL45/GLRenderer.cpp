/**
 * @file GLRenderer.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See GLRenderer.h.
 */

#include <cmath>
#include <cstring>
#include <string>
#include <memory>

#include "tinyxml2.h"
#include "Preprocess.h"
#include "GL/GL.h"

#include "Display/GL45/GLDisplayManager.h"
#include "Display/GL45/GLProgram.h"
#include "Display/GL45/GLRenderer.h"

using std::string;
using std::shared_ptr;
using std::weak_ptr;
using std::make_shared;
using tinyxml2::XMLElement;
using tinyxml2::XMLError;

namespace Sim {

	GLRenderer::GLRenderer ()
	: _owner (nullptr), _hither (1.), _yon (1500.), _fov (45.),
		_mouseX (0), _mouseY (0), _mouseButton (0), _numLights (0)
	{
		// make background into black
		for (unsigned int i = 0; i < 3; ++i){
			_background [i] = 0.;
		}

		// make projection and modelview matrices into identity matrices
		for (unsigned int i = 0; i < 16; ++i){
			_projection [i] = 0.;
		}
		for (unsigned int i = 0; i < 16; ++i){
			_modelview [i] = 0.;
		}
		_projection [0] = _projection [5] = _projection [10] = _projection [15] = 1.;
		_modelview [0] = _modelview [5] = _modelview [10] = _modelview [15] = 1.;

		// place camera at origin
		for (unsigned int i = 0; i < 3; ++i){
			_cameraPosition [i] = 0.;
		}

		// make camera scaling factor to be identity
		for (unsigned int i = 0; i < 3; ++i){
			_cameraScales [i] = 1.;
		}

		LOG ("OpenGL Renderer constructed");
	}

	GLRenderer::~GLRenderer ()
	{}

	bool GLRenderer::Initialize (XMLElement& elem)
	{
		XMLElement* relem = elem.FirstChildElement ("Background");
		if (relem != nullptr){
			SetWindowBackground (*relem);
		}
		relem = nullptr;

		relem = elem.FirstChildElement ("Camera");
		if (relem != nullptr){
			SetCameraPosition (*relem);
		}
		relem = nullptr;

		relem = elem.FirstChildElement ("DirectionalLights");
		if (relem != nullptr){
			if (!SetDirectionalLights (*relem)){
				LOG_ERROR ("Could not initialize directional lights");
				return false;
			}
		}

		EnableGLAttribs ();
		CheckGLVersion ();
		return true;
	}

	void GLRenderer::Update ()
	{
		// clear from previous frame
		glClearColor (_background [0], _background [1], _background [2], 0.);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GLRenderer::Cleanup ()
	{
	}

	GLuint GLRenderer::AddProgram (const char* name, const char* location)
	{
		// check for existing program
		auto search = _programs.find (name);
		if (search != _programs.end ()){
			return search->second->Id ();
		}

		// load a new program
		_programs [name] = make_shared <GLProgram> (name, location);
		if (!_programs [name]->Load ()){
			LOG_ERROR ("Could not load GLSL program " << name << " at " << location);
			return 0;
		}
		return _programs [name]-> Id ();
	}

	GLuint GLRenderer::GetProgramId (const char* name) const
	{
#		ifndef NDEBUG
		for (const auto &nm : _programs){
			if (!nm.first.compare (name)){
				return nm.second->Id ();
			}
		}
		LOG_ERROR ("No GLSL program with the name " << name << " found. Returning 0");
		return 0;
#		else
		return _programs [name]->Id ();
#		endif
	}

	bool GLRenderer::ReloadProgram (GLuint id)
	{
		for (const auto &p : _programs){
			if (id == p.second->Id ()){
				return p.second->Load ();
			}
		}
		LOG_ERROR ("No program with id " << id << " found");
		return false;
	}

	void GLRenderer::CheckGLVersion ()
	{
		const GLubyte *version = glGetString (GL_VERSION);
		LOG ("GL version: " << version);

		char vch [4];
		vch [0] = static_cast <char> (version [0]);
		vch [1] = static_cast <char> (version [2]);
		vch [2] = '0';
		vch [3] ='\0';

		int versionNum = atoi (vch);
		if (versionNum < 330){
      LOG_WARNING ("Current GL version too old to support Chimera");
		}
	}

	void GLRenderer::SetWindowBackground (XMLElement& elem)
	{
		XMLElement* celem = elem.FirstChildElement ("Color");
		if (celem != nullptr){

			Real r = 0., g = 0., b = 0.;
#			ifdef SIM_DOUBLE_PRECISION
			celem->QueryDoubleAttribute ("R", &r);
			celem->QueryDoubleAttribute ("G", &g);
			celem->QueryDoubleAttribute ("B", &b);
#			else
			celem->QueryFloatAttribute ("R", &r);
			celem->QueryFloatAttribute ("G", &g);
			celem->QueryFloatAttribute ("B", &b);
#			endif

			_background [0] = static_cast <GLreal> (r);
			_background [1] = static_cast <GLreal> (g);
			_background [2] = static_cast <GLreal> (b);
		}
	}

	void GLRenderer::SetCameraPosition (XMLElement& elem)
	{
		XMLElement* celem = elem.FirstChildElement ("Position");
		if (celem != nullptr){

			Real x = 0., y = 0., z = 0.;
#			ifdef SIM_DOUBLE_PRECISION
			celem->QueryDoubleAttribute ("X", &x);
			celem->QueryDoubleAttribute ("Y", &y);
			celem->QueryDoubleAttribute ("Z", &z);
#			else
			celem->QueryFloatAttribute ("X", &x);
			celem->QueryFloatAttribute ("Y", &y);
			celem->QueryFloatAttribute ("Z", &z);
#			endif

			_cameraPosition [0] = static_cast <GLreal> (x);
			_cameraPosition [1] = static_cast <GLreal> (y);
			_cameraPosition [2] = static_cast <GLreal> (z);
		}
	}

	bool GLRenderer::SetDirectionalLights (XMLElement& elem)
	{
		unsigned int count = 0;
		elem.QueryUnsignedAttribute ("Count", &count);
		if (!count){
			LOG_ERROR ("Directional light count not specified");
			return false;
		}
		_numLights = count;

		XMLElement* lelem = elem.FirstChildElement ("Light");
		if (lelem == nullptr){
			LOG_ERROR ("Directional Lights specified but no \'Light\' element retrieved");
			return false;
		}
		_directionalLights = shared_ptr <DirectionalLight> (new DirectionalLight [count], DeleteArray <DirectionalLight> ());

		unsigned int i = 0;
		while (lelem != nullptr && i < count){

			DirectionalLight light;
			XMLElement* celem = lelem->FirstChildElement ("Direction");
			if (celem != nullptr){
				Real x = 0., y = 0., z = 0;
#				ifndef SIM_DOUBLE_PRECISION
				celem->QueryFloatAttribute ("X", &x);
				celem->QueryFloatAttribute ("Y", &y);
				celem->QueryFloatAttribute ("Z", &z);
#				else
				celem->QueryDoubleAttribute ("X", &x);
				celem->QueryDoubleAttribute ("Y", &y);
				celem->QueryDoubleAttribute ("Z", &z);
#				endif
				light._direction [0] = static_cast <GLreal> (x);
				light._direction [1] = static_cast <GLreal> (y);
				light._direction [2] = static_cast <GLreal> (z);
			}

			celem = lelem->FirstChildElement ("Ambient");
			if (celem != nullptr){
				Real r = 0., g = 0., b = 0;
#				ifndef SIM_DOUBLE_PRECISION
				celem->QueryFloatAttribute ("R", &r);
				celem->QueryFloatAttribute ("G", &g);
				celem->QueryFloatAttribute ("B", &b);
#				else
				celem->QueryDoubleAttribute ("R", &r);
				celem->QueryDoubleAttribute ("G", &g);
				celem->QueryDoubleAttribute ("B", &b);
#				endif
				light._ambient [0] = static_cast <GLreal> (r);
				light._ambient [1] = static_cast <GLreal> (g);
				light._ambient [2] = static_cast <GLreal> (b);
			}

			celem = lelem->FirstChildElement ("Diffuse");
			if (celem != nullptr){
				Real r = 0., g = 0., b = 0;
#				ifndef SIM_DOUBLE_PRECISION
				celem->QueryFloatAttribute ("R", &r);
				celem->QueryFloatAttribute ("G", &g);
				celem->QueryFloatAttribute ("B", &b);
#				else
				celem->QueryDoubleAttribute ("R", &r);
				celem->QueryDoubleAttribute ("G", &g);
				celem->QueryDoubleAttribute ("B", &b);
#				endif
				light._diffuse [0] = static_cast <GLreal> (r);
				light._diffuse [1] = static_cast <GLreal> (g);
				light._diffuse [2] = static_cast <GLreal> (b);
			}

			celem = lelem->FirstChildElement ("Specular");
			if (celem != nullptr){
#				ifndef SIM_DOUBLE_PRECISION
				celem->QueryFloatText (&light._specular);
#				else
				celem->QueryDoubleAttribute ("Value", &light._specular);
#				endif
			}

			celem = lelem->FirstChildElement ("Exponent");
			if (celem != nullptr){
#				ifndef SIM_DOUBLE_PRECISION
				celem->QueryFloatText (&light._exponent);
#				else
				celem->QueryDoubleAttribute ("Value", &light._exponent);
#				endif
			}

			_directionalLights.get () [i] = light;
			++i; lelem = lelem->NextSiblingElement ("Light");
		}
		if (i < count){
			_numLights = i;
			LOG_WARNING ("Directional Light \'Count\'" << count << " does not correspond with number of lights specified - " << i);
		}
		return true;
	}

	void GLRenderer::EnableGLAttribs ()
	{
		glEnable (GL_ALPHA_TEST);
		glEnable (GL_AUTO_NORMAL);

		glEnable (GL_BLEND);
		glShadeModel (GL_SMOOTH);
		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glEnable (GL_CULL_FACE);
		glCullFace (GL_BACK);

		glEnable (GL_DEPTH_TEST);
		glDepthFunc (GL_LESS);

		glEnable (GL_LIGHTING);
		glEnable (GL_RESCALE_NORMAL);

		glEnable (GL_POINT_SMOOTH);
		glEnable (GL_LINE_SMOOTH);
		glEnable (GL_POLYGON_SMOOTH);
	}

	void GLRenderer::Mouse (unsigned int b, int x, int y)
	{
		_mouseButton = b;
		_mouseX = x;
		_mouseY = y;
	}

	// method for rotational mouse motion
	void GLRenderer::LeftMouseMotion (int x, int y)
	{
		// record change
		Real deltaX = static_cast <Real> (x - _mouseX);
		Real deltaY = static_cast <Real> (y - _mouseY);

		// update position
		_mouseX = x;
		_mouseY = y;

		deltaX *= 0.01; // to damp motion
		deltaY *= 0.01; // to damp motion

		Real mag = static_cast <Real> (sqrt (deltaX*deltaX + deltaY*deltaY));
		deltaX /= mag;
		deltaY /= mag;
		Real cosVal = static_cast <Real> (cos (180.* mag/ _owner->WindowWidth ()));
		Real sinVal = static_cast <Real> (sqrt (1. - cosVal*cosVal));

		Real tr00 = deltaY * deltaY * (1. - cosVal) + cosVal;
		Real tr01 = deltaY * deltaX * (1. - cosVal);
		Real tr02 = deltaX * sinVal;
		Real tr11 = deltaX * deltaX * (1. - cosVal) + cosVal;
		Real tr12 = -deltaY * sinVal;
		Real tr22 = cosVal;

		Real result [16];
		result [0] = tr00 * _modelview [0] + tr01 * _modelview [1] + tr02 * _modelview [2];
		result [1] = tr01 * _modelview [0] + tr11 * _modelview [1] + tr12 * _modelview [2];
		result [2] = -tr02 * _modelview [0] - tr12 * _modelview [1] + tr22 * _modelview [2];
		result [3] = _modelview [3];

		result [4] = tr00 * _modelview [4] + tr01 * _modelview [5] + tr02 * _modelview [6];
		result [5] = tr01 * _modelview [4] + tr11 * _modelview [5] + tr12 * _modelview [6];
		result [6] = -tr02 * _modelview [4] - tr12 * _modelview [5] + tr22 * _modelview [6];
		result [7] = _modelview [7];

		result [8] = tr00 * _modelview [8] + tr01 * _modelview [9] + tr02 * _modelview [10];
		result [9] = tr01 * _modelview [8] + tr11 * _modelview [9] + tr12 * _modelview [10];
		result [10] = -tr02 * _modelview [8] - tr12 * _modelview [9] + tr22 * _modelview [10];
		result [11] = _modelview [11];

		result [12] = tr00 * _modelview [12] + tr01 * _modelview [13] + tr02 * _modelview [14];
		result [13] = tr01 * _modelview [12] + tr11 * _modelview [13] + tr12 * _modelview [14];
		result [14] = -tr02 * _modelview [12] - tr12 * _modelview [13] + tr22 * _modelview [14];
		result [15] = _modelview [15];

		memcpy (_modelview, result, 16*sizeof (Real));
	}

	// method for zoom mouse motion
	void GLRenderer::RightMouseMotion (int x, int y)
	{
		// record change
		Real deltaY = static_cast <Real> (y - _mouseY);

		// update position
		_mouseX = x;
		_mouseY = y;

		Real tr23 = -_cameraScales [2] * deltaY;

		Real result [4];
		result [0] = _modelview [2] + tr23 * _modelview [3];
		result [1] = _modelview [6] + tr23 * _modelview [7];
		result [2] = _modelview [10] + tr23 * _modelview [11];
		result [3] = _modelview [14] + tr23 * _modelview [15];

		for (int i = 0; i < 4; ++i){
			_modelview [4*i + 2] = result [i];
		}
		for (int i = 0; i < 16; ++i){
			_modelview [i] /= _modelview [15];
		}
	}

	// method panning mouse motion
	void GLRenderer::MiddleMouseMotion (int x, int y)
	{
		// record change
		Real deltaX = static_cast <Real> (x - _mouseX);
		Real deltaY = static_cast <Real> (y - _mouseY);

		// update position
		_mouseX = x;
		_mouseY = y;

		Real tr03 = -_cameraScales [0] * deltaX;
		Real tr13 = -_cameraScales [1] * deltaY;

		Real result [8];
		result [0] = _modelview [0] + tr03 * _modelview [3];
		result [1] = _modelview [4] + tr03 * _modelview [7];
		result [2] = _modelview [8] + tr03 * _modelview [11];
		result [3] = _modelview [12] + tr03 * _modelview [15];
		result [4] = _modelview [1] + tr13 * _modelview [3];
		result [5] = _modelview [5] + tr13 * _modelview [7];
		result [6] = _modelview [9] + tr13 * _modelview [11];
		result [7] = _modelview [13] + tr13 * _modelview [15];

		for (int i = 0; i < 4; ++i){
			_modelview [4*i] = result [i];
		}
		for (int i = 0; i < 4; ++i){
			_modelview [4*i + 1] = result [4 + i];
		}
		for (int i = 0; i < 16; ++i){
			_modelview [i] /= _modelview [15];
		}
	}

	void GLRenderer::UpdateProjection ()
	{
		Real top = _hither * static_cast <Real> (tan (M_PI * _fov/ 360.));
		Real bottom = -top;
		Real aspect = static_cast <Real> (_owner->WindowWidth ())/ _owner->WindowHeight ();
		Real left = bottom * aspect;
		Real right = top * aspect;

		_projection [0] = 2 * _hither / (right - left);
		_projection [2] =  (right + left)/ (right - left);
		_projection [5] = 2 * _hither / (top - bottom);
		_projection [6] =  (top + bottom)/ (top - bottom);
		_projection [10] = - (_yon + _hither)/ (_yon - _hither);
		_projection [11] = -2 * _hither * _hither/ (_yon - _hither);
		_projection [14] = -1.0;
	}

	void GLRenderer::UpdateModelview ()
	{
		// calculate the look-at position
		Vector bv0 (_bounds [0]), bv1 (_bounds [7]);
		Vector3 at ((bv0 [0] + bv1 [0])* 0.5, (bv0 [1] + bv1 [1])* 0.5, bv0 [2]);

		// get the maximum XY extent of the view volume
		Real maxExtent = 0.0;
		if (bv1 [0] - bv0 [0] > bv1 [1] - bv0 [1]){
			maxExtent = bv1 [0] - bv0 [0];
		} else {
			maxExtent = bv1 [1] - bv0 [1];
		}
		maxExtent *= 0.5; // we use half of the max-extent for our calculations

		// set camera position such that it forms angle designated by _fov - field of view
		Vector3 camera (at);
		Real distance = maxExtent/ static_cast <Real> (tan (M_PI * _fov / 360.));
		camera [2] += distance;

		// form view, up and right vectors
		Vector3 view (camera), right, up;
		camera -= at;
		view.Normalize ();
		Vector3 (0., 1., 0.).NormalizedCross (view, right);
		view.Cross (right, up);

		// assign model view matrix factors
		_modelview [0] = right [0];
		_modelview [4] = right [1];
		_modelview [8] = right [2];

		_modelview [1] = up [0];
		_modelview [5] = up [1];
		_modelview [9] = up [2];

		_modelview [2] = view [0];
		_modelview [6] = view [1];
		_modelview [10] = view [2];

		_modelview [12] = -right.Dot (camera);
		_modelview [13] = -up.Dot (camera);
		_modelview [14] = -view.Dot (camera);

		_cameraPosition [0] = camera [0];
		_cameraPosition [1] = camera [1];
		_cameraPosition [2] = camera [2];

		_cameraScales [0] = 0.01*_modelview [12];
		_cameraScales [1] = 0.01*_modelview [13];
		_cameraScales [2] = 0.01*_modelview [14];
	}
}

