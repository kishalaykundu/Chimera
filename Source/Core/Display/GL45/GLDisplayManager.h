/**
 * @file GLDisplayManager.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The OpenGL/GLX based Display manager.
 */
#pragma once

#include <memory>
#include "tinyxml2.h"

#include "GL/GL.h"
#include "X11/X11.h"

#include "Display/DisplayManager.h"

namespace Sim {

	class GLRenderer;

	class GLDisplayManager : public DisplayManager {

		protected:
			// GLX Window Attributes
			int _top;
			int _left;

			unsigned int _width;
			unsigned int _height;
			int _colorDepth;

			::GLXContext _context;
			::Window _window;
			::Display *_display;
			::XWindowAttributes _attributes;
			::XEvent _event;

			std::unique_ptr <GLRenderer> _renderer;

		private: // forbiddent copy constructor and assignment operator
			GLDisplayManager (const GLDisplayManager&);
			GLDisplayManager& operator = (const GLDisplayManager&);

		public:
			GLDisplayManager ();
			~GLDisplayManager ();

			virtual bool Initialize (const char* config) override;
			virtual void Cleanup () override;

			// GLX-related methods
			void MakeContextCurrent ();
			void ReleaseContext ();
			void WindowResize (unsigned int, unsigned int);
			void SwapBuffers ();
			void WindowMetrics (unsigned int&, unsigned int&, int&, int&, int&) const;
			unsigned int WindowWidth () const;
			unsigned int WindowHeight () const;
			unsigned int WindowColorDepth () const;

			unsigned int AddProgram (const char* name, const char* location);
			unsigned int GetProgramId (const char* name) const;
			bool ReloadProgram (unsigned id);

		protected:
			void UpdateWindow ();
			void DestroyWindow ();
			bool InitializeGLX ();

			bool InitializeRenderer (tinyxml2::XMLElement&);
			bool InitializeWindow (tinyxml2::XMLElement&);
	};
}
