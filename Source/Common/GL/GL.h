/**
 * @file GL.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * Platform-dependent OpenGL header files.
 */
#pragma once

#define GL_GLEXT_PROTOTYPES 1

#if defined (__linux__) || defined (__APPLE__) || defined (MACOSX)
#	define GLX_GLXEXT_PROTOTYPES 1
#	define SIM_GLX_MIN_MAJOR_VERSION 1
#	define SIM_GLX_MIN_MINOR_VERSION 4
#endif

extern "C" {

#if defined( __APPLE__ ) || defined( MACOSX )
#	include <OpenGL/gl.h>
#	include <OpenGL/glext.h>
#	include <OpenGL/glx.h>
#	include <OpenGL/glxext.h>
#else
#	include <GL/glx.h>
#	include <GL/glxext.h>
#	include <GL/gl.h>
#	include <GL/glext.h>
#endif

}

#define SIM_GL_MIN_MAJOR_VERSION 4
#define SIM_GL_MIN_MINOR_VERSION 5

#define SIM_GLX_MIN_MAJOR_VERSION 1
#define SIM_GLX_MIN_MINOR_VERSION 4

#if defined (__linux__) || defined (__APPLE__) || defined (MACOSX)
	typedef GLXContext GPU_CONTEXT;
#endif

#if !defined(NDEBUG) && defined(SIM_GL_DEBUG)
#	include <cstring> // basename (...) is defined in cstring
#	include <iostream>
	void APIENTRY GLDebugMessage (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
			const GLchar* message, const void* parameters)
	{
		std::cerr << basename( __FILE__ ) << "[" << __LINE__ << "]: GL error:" << std::endl;
		std::cerr << "ID - [" << id << "] Type - ";
		switch (type) {
			case GL_DEBUG_TYPE_ERROR:
				std::cerr << "ERROR";
				break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
				std::cerr << "DEPRECATED_BEHAVIOR";
				break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
				std::cerr << "UNDEFINED_BEHAVIOR";
				break;
			case GL_DEBUG_TYPE_PORTABILITY:
				std::cerr << "PORTABILITY";
				break;
			case GL_DEBUG_TYPE_PERFORMANCE:
				std::cerr << "PERFORMANCE";
				break;
			case GL_DEBUG_TYPE_OTHER:
				std::cerr << "OTHER";
				break;
		}
		std::cerr << " Severity - ";
		switch (severity) {
			case GL_DEBUG_SEVERITY_LOW:
				std::cerr << "LOW";
				break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				std::cerr << "MEDIUM";
				break;
			case GL_DEBUG_SEVERITY_HIGH:
				std::cerr << "HIGH";
				break;
		}
		std::cerr << std::endl;
		std::cerr << message << std::endl;
	}
#endif
