/**
 * @file GLDisplayManager.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See GLDisplayManager.h.
 */
#include <memory>

#include "tinyxml2.h"
#include "Config.h"
#include "Preprocess.h"

#include "InputParser.h"
#include "Driver.h"
#include "Display/GL45/GLRenderer.h"
#include "Display/GL45/GLDisplayManager.h"

using std::make_unique;
using tinyxml2::XMLElement;

namespace Sim {

	// default constructor
	GLDisplayManager::GLDisplayManager ()
	: _top (0), _left (0), _width (256), _height (256), _colorDepth (16),
		_display (nullptr), _context (0), _contextAttributes (nullptr)
	{
		LOG ("OpenGL display manager constructed");
	}

	GLDisplayManager::~GLDisplayManager ()
	{
		Cleanup ();
		LOG ("OpenGL display manager destroyed.");
	}

	GLDisplayManager::GLDisplayManager (const GLDisplayManager& dm) {}

	GLDisplayManager& GLDisplayManager::operator = (const GLDisplayManager& dm) {return *this;}

	bool GLDisplayManager::Initialize (const char* configfile)
	{
		// read configuration file
		InputParser parser;
		if (!parser.Initialize (configfile, "GLConfig")){
			LOG_ERROR ("Could not initialize parser for " << configfile);
			return false;
		}

		XMLElement* element = parser.GetElement ("Window");
		if (element == nullptr){
			LOG_ERROR ("No window profile specified in " << configfile);
			return false;
		}
		if (!InitializeWindow (*element)){
			LOG_ERROR ("Could not initialize X Window from " << configfile);
			Cleanup ();
			return false;
		}
		element = nullptr;

		element = parser.GetElement ("Renderer");
		if (element == nullptr){
			LOG_ERROR ("No renderer profile in " << configfile);
			Cleanup ();
			return false;
		}
		if (!InitializeRenderer (*element)){
			LOG_ERROR ("Could not initialize renderer from " << configfile);
			Cleanup ();
			return false;
		}
		glFinish ();
		LOG ("OpenGL 4.5 display manager initialized");
		return true;
	}

	void GLDisplayManager::Cleanup ()
	{
		_renderer.reset ();

		// last line to destroy display window
		DestroyWindow ();
	}

	// GLX-Window related methods
	void GLDisplayManager::WindowResize (unsigned int width, unsigned int height)
	{
		_width = width;
		_height = height;
		XResizeWindow (_display, _window, _width, _height);
	}
	void GLDisplayManager::SwapBuffers ()
	{
		glXSwapBuffers (_display, _window);
	}
	void GLDisplayManager::WindowMetrics (unsigned int &width, unsigned int &height, int &depth, int &top, int &left) const
	{
		width = _width;
		height = _height;
		depth = _colorDepth;

		top = _top;
		left = _left;
	}
	unsigned int GLDisplayManager::WindowWidth () const {return _width;}
	unsigned int GLDisplayManager::WindowHeight () const {return _height;}
	unsigned int GLDisplayManager::WindowColorDepth () const {return static_cast <unsigned int> (_colorDepth);}

	Display* GLDisplayManager::GetDisplay () const {return _display;}
	GLXFBConfig GLDisplayManager::GetConfig () const {return _config;}
	GLXContext GLDisplayManager::GetContext () const {return _context;}
	int* GLDisplayManager::GetContextAttributes () const {return _contextAttributes;}
	void GLDisplayManager::MakeContextCurrent ()
	{
		glXMakeContextCurrent (_display, _window, _window, _context);
	}
	void GLDisplayManager::ReleaseContext ()
	{
    glXMakeContextCurrent (_display, None, None, NULL);
	}

	GLuint GLDisplayManager::AddProgram (const char* name, const char* location)
	{
		return _renderer->AddProgram (name, location);
	}
	GLuint GLDisplayManager::GetProgramId (const char* name) const
	{
		return _renderer->GetProgramId (name);
	}
	bool GLDisplayManager::ReloadProgram (GLuint id)
	{
		return _renderer->ReloadProgram (id);
	}

	// handle X Window events
	void GLDisplayManager::UpdateWindow ()
	{
	 	XNextEvent(_display, &_event);

	 	switch (_event.type) {

	 	case Expose:
	 	{
			XGetWindowAttributes(_display, _window, &_attributes);
			if (_width != static_cast <unsigned int> (_attributes.width) ||
					_height != static_cast <unsigned int> (_attributes.height)){
				WindowResize (_attributes.width, _attributes.height);
				_renderer->UpdateProjection ();
			}
			break;
	 	}
	 	case ButtonPress:
	 	{
	 		_renderer->Mouse (_event.xbutton.button, _event.xbutton.x, _event.xbutton.y);
	 		break;
	 	}
	 	case MotionNotify:
	 	{
	 		switch (_event.xmotion.state){
	 			case Button1Mask:
	 				_renderer->LeftMouseMotion (_event.xmotion.x, _event.xmotion.y);
	 				break;
	 			case Button2Mask:
	 				_renderer->RightMouseMotion (_event.xmotion.x, _event.xmotion.y);
	 				break;
	 			case Button3Mask:
	 				_renderer->MiddleMouseMotion (_event.xmotion.x, _event.xmotion.y);
	 				break;
	 		}
	 		break;
	 	}
	 	case KeyPress :
	 	{
			char buff [20];
			unsigned int buffsize = 20;
			KeySym key;
			XComposeStatus compose;
			XLookupString (&_event.xkey, buff, buffsize, &key, &compose);

			LOG ("Input Key: " << buff);
			switch (key) {

			case XK_q: case XK_Q: case XK_Escape:
			{
		 		Driver::Instance ().Quit ();
				break;
			}

			default:
				break;
			}
	 		break;
	 	}
	 	default:
	 		LOG_ERROR ("Unrecognized XEvent");
	 		break;
	 	}
	}

	// method to destroy display window
	void GLDisplayManager::DestroyWindow ()
	{
    glXMakeContextCurrent (_display, None, None, NULL);
 		glXDestroyContext (_display, _context);
 		XDestroyWindow (_display, _window);
 		XCloseDisplay (_display);
	}

	// method to initialize the GLX rendering system
	bool GLDisplayManager::InitializeGLX ()
	{
		_display = XOpenDisplay (NULL);
		if (!_display){
			LOG_ERROR ("Could not open X Display system");
			return false;
		}

	  // check for minimum version of GLX
	  int major = 0, minor = 0;
	  if (!glXQueryVersion (_display, &major, &minor)){
	    LOG_ERROR("Could not get GLX version for display");
	    return false;
	  }
	  if (((major == SIM_GLX_MIN_MAJOR_VERSION) && (minor < SIM_GLX_MIN_MINOR_VERSION)) || (major < SIM_GLX_MIN_MAJOR_VERSION)){
	    LOG_ERROR("GLX version: " << major << "." << minor << ". Minimum required: 1.4");
	    return false;
	  }

	  // framebuffer configuration attributes
	  int attributes [] = {
	  		GLX_X_RENDERABLE, True,
	  		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	  		GLX_RENDER_TYPE, GLX_RGBA_BIT,
				GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
	      GLX_RED_SIZE, 8,
				GLX_GREEN_SIZE, 8,
				GLX_BLUE_SIZE, 8,
				GLX_ALPHA_SIZE, 8,
	      GLX_DEPTH_SIZE, 24,
				GLX_STENCIL_SIZE, 8,
				GLX_DOUBLEBUFFER, True,
				None };

	  // initialize framebuffer config
	  int fbCount = 0;
	  GLXFBConfig* fbConfig = glXChooseFBConfig (_display, DefaultScreen(_display), attributes, &fbCount);
	  if (!fbConfig){
	  	LOG_ERROR ("Could not retrieve GLX framebuffer config");
	  	return false;
	  }

	  // choose the framebuffer config with most samples per pixel
	  int bestFB = -1, worstFB = -1, bestNumSamples = -1, worstNumSamples = -1;
	  for (int i = 0; i < fbCount; ++i){
	  	XVisualInfo* info = glXGetVisualFromFBConfig (_display, fbConfig [i]);
	  	if (info){
	  		int buffers, samples;
	  		glXGetFBConfigAttrib (_display, fbConfig [i], GLX_SAMPLE_BUFFERS, &buffers);
	  		glXGetFBConfigAttrib (_display, fbConfig [i], GLX_SAMPLES, &samples);

	  		if (bestFB < 0 || (buffers && samples > bestNumSamples)){
	  			bestFB = i;
	  			bestNumSamples = samples;
	  		}
	  		if (worstFB < 0 || !buffers || samples < worstNumSamples){
	  			worstFB = i;
	  			worstNumSamples = samples;
	  		}
	  	}
	  	XFree (info);
	  }
	  _config = fbConfig [bestFB];
	  XFree (fbConfig);

	  // get an X Visual
	  XVisualInfo* info = glXGetVisualFromFBConfig (_display, _config);
	  XSetWindowAttributes swa;
	  Colormap cmap;
	  swa.colormap = cmap = XCreateColormap(_display, RootWindow (_display, info->screen), info->visual, AllocNone );
	  swa.background_pixmap = None;
	  swa.border_pixel = 0;
	  swa.event_mask = StructureNotifyMask;

	  // create display window
	  _colorDepth = info->depth;
	  _window = XCreateWindow (_display, RootWindow (_display, info->screen), _top, _left, _width, _height,
	  		0, _colorDepth, InputOutput, info->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
	  if (!_window){
	  	LOG_ERROR ("Could not create display window");
	  	XFree (info);
	  	return false;
	  }
	  XFree (info);

	  // form window title
		XStoreName (_display, _window, "Chimera GLX Window");

		// map window to display
		XMapWindow (_display, _window);

		// create new context
#		ifndef NDEBUG
		_contextAttributes = new int [7];
		_contextAttributes [4] = GLX_CONTEXT_FLAGS_ARB;
		_contextAttributes [5] = GLX_CONTEXT_DEBUG_BIT_ARB;
		_contextAttributes [6] = None;
#		else
		_contextAttributes = new int [5];
		_contextAttributes [4] = None;
#		endif
		_contextAttributes [0] = GLX_CONTEXT_MAJOR_VERSION_ARB;
		_contextAttributes [1] = SIM_GL_MIN_MAJOR_VERSION;
		_contextAttributes [2] = GLX_CONTEXT_MINOR_VERSION_ARB;
		_contextAttributes [3] = SIM_GL_MIN_MINOR_VERSION;

		_context = glXCreateContextAttribsARB (_display, _config, 0, true, _contextAttributes);
		if (!_context){
			LOG_ERROR ("Could not create GLX context");
		  return false;
		}

		XSync (_display, False);

	  if (glXIsDirect (_display, _context) == True){
	  	LOG ("Direct GL rendering context available");
	  } else {
	  	LOG ("Indirect GL rendering context available");
	  }

	  MakeContextCurrent ();
	  if (gl3wInit ()){
	  	LOG_ERROR ("GL3W and OpenGL could not be initialized");
	  	return false;
	  }
	  if (!gl3wIsSupported(4, 5)){
	  	LOG_ERROR ("OpenGL 4.5 not supported");
	  	return false;
	  }

		return true;
	}

	// Config-based GL Renderer initialization method
	bool GLDisplayManager::InitializeRenderer (XMLElement& config)
	{
		_renderer = make_unique <GLRenderer> ();
		if (! _renderer->Initialize (config)){
			LOG_ERROR ("Renderer could not be initialized");
			return false;
		}
		_renderer->_owner = this;
		return true;
	}

	// GLX Window initialization method
	bool GLDisplayManager::InitializeWindow (XMLElement& config)
	{
		XMLElement* elem = config.FirstChildElement ("Position");
		if (elem != nullptr){
			elem->QueryIntAttribute ("Top", &_top);
			elem->QueryIntAttribute ("Left", &_left);
		}
		elem = config.FirstChildElement ("Dimensions");
		if (elem != nullptr){
			elem->QueryUnsignedAttribute ("Width", &_width);
			elem->QueryUnsignedAttribute ("Height", &_height);
		}
		elem = config.FirstChildElement ("PixelDepth");
		elem->QueryIntText (&_colorDepth);

		return InitializeGLX ();
	}

}
