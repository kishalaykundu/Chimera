/*
 * Program to query the version of GL installed through GLX. This program can
 * be used if glxinfo (only available through the Mesa package) is not installed.
 */
#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "GL/GL.h"
#include "X11/X11.h"

using std::cout;
using std::endl;

int main ()
{
	
	Display* display = XOpenDisplay (NULL);
	if (!display) {
		cout << "Unable to open connection to X server" << endl;
		exit (EXIT_FAILURE);
	}
	Window root = DefaultRootWindow (display);

	GLint major = 0, minor = 0;

	// check for minimum GLX version
	if (!glXQueryVersion (display, &major, &minor)){
		cout << "Could not find X-server's supported GLX version" << endl;
		exit (EXIT_FAILURE);
	}
	cout << "GLX extension version: " << major << "." << minor << endl;

	if (!glXQueryExtension (display, NULL, NULL)){
		cout << "X server does not support installed GLX - supported version: " << major << "." << minor << endl;
	}

	GLint attributes [] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
	XVisualInfo *info = glXChooseVisual (display, 0, attributes);

	// context is necessary in order to get the GL versions below
	GLXContext context = glXCreateContext (display, info, NULL, GL_TRUE);
	glXMakeCurrent (display, root, context);

	cout << "OpenGL version: " << glGetString (GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString (GL_SHADING_LANGUAGE_VERSION) << endl;
	cout << "GL Renderer: " << glGetString (GL_RENDERER) << endl;

	exit (EXIT_SUCCESS);
}

