/**
 * @file GLProgram.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See GLProgram.h
 */

#include <string>
#include <iostream>
#include <fstream>

#include "Preprocess.h"
#include "GL/GL.h"

#include "Display/GL45/GLProgram.h"

using std::string;
using std::ifstream;
using std::ios;

namespace Sim {

	GLProgram::GLProgram (const string& name, const string& location)
	: _id (0), _name (name), _location (location)
	{
		// change location to contain the name/prefix
		if (_location [_location.size () - 1] != '/'){
			_location.append ("/");
		}
		_location.append (name);
	}

	GLProgram::~GLProgram ()
	{
		Unload ();
	}

	/**
	 * Method used to load and reload GLSL program files. This method
	 * requires, as does GLSL, a vertex shader and a fragment shader.
	 * Geometry, tessellation and compute shaders are optional. It is
	 * left to the user to correctly specify the names of these files
	 * at the location they provide.
	 * All shader files are expected to have the same name, with the
	 * only variations being in their extensions. All shader files are
	 * expected to have the following extensions:
	 * 1. Vertex shaders - *.vs
	 * 2. Geometry shaders - *.gs
	 * 3. Tessellation control shaders - *.tes
	 * 4. Tessellation evaluation shaders - *.tcs
	 * 5. Fragment shaders - *.fs
	 * 6. Compute shaders - *.cs
	 */
	bool GLProgram::Load ()
	{
		if (!Unload ()){
			return false;
		}
    GLenum error;

    _id = glCreateProgram ();
    if ((error = glGetError ()) != GL_NO_ERROR){
      LOG_ERROR ("GL error: " << error);
      return false;
    }

    GLuint id;

    // load vertex shader
    string shader (_location + ".vs");
    if ( !LoadShader (GL_VERTEX_SHADER, shader, id)){
    	LOG_ERROR (shader.c_str () << " could not be initialized");
      return false;
    }
    glAttachShader (_id, id);
    if ((error = glGetError ()) != GL_NO_ERROR){
    	LOG_ERROR ("GL error: " << error);
      return false;
    }

    // load geometry shader (optional)
    shader = string(_location + ".gs");
    ifstream is;
    is.open (shader.c_str (), ios::binary);
    if (!is.fail ()){
  		if ( !LoadShader (GL_GEOMETRY_SHADER_ARB, shader, id)){
  			LOG_ERROR (shader.c_str () << " could not be initialized");
        return false;
  		}
  		glAttachShader (_id, id);
  		if ((error = glGetError ()) != GL_NO_ERROR){
  			LOG_ERROR ("GL error: " << error);
  			return false;
  		}
    }

#ifdef GL_ARB_tessellation_shader
    // load tessellation control shader (optional)
    shader = string(_location + ".tcs");
    is.open (shader.c_str (), ios::binary);
    if (!is.fail ()){
  		if ( !LoadShader (GL_TESS_CONTROL_SHADER, shader, id)){
  			LOG_ERROR (shader.c_str () << " could not be initialized");
        return false;
  		}
  		glAttachShader (_id, id);
  		if ((error = glGetError ()) != GL_NO_ERROR){
        LOG ("GL error: " << error);
  			return false;
  		}
    }

    // load tessellation evaluation shader (optional)
    shader = string(_location + ".tes");
    is.open (shader.c_str (), ios::binary);
    if (!is.fail ()){
  		if ( !LoadShader (GL_TESS_EVALUATION_SHADER, shader, id)){
  			LOG_ERROR (shader.c_str () << " could not be initialized");
        return false;
  		}
  		glAttachShader (_id, id);
  		if ((error = glGetError ()) != GL_NO_ERROR){
  			LOG_ERROR ("GL error: " << error);
  			return false;
  		}
    }
#endif

    // load fragment shader
    shader = string(_location + ".fs");
    if ( !LoadShader (GL_FRAGMENT_SHADER, shader, id)){
    	LOG_ERROR (shader.c_str () << " could not be initialized");
      return false;
    }
    glAttachShader (_id, id);
    if ((error = glGetError ()) != GL_NO_ERROR){
    	LOG_ERROR ("GL error: " << error);
      return false;
    }

#ifdef GL_ARB_compute_shader
    // load compute shader (optional)
    shader = string(_location + ".cs");
    is.open (shader.c_str (), ios::binary);
    if (!is.fail ()){
  		if ( !LoadShader (GL_COMPUTE_SHADER, shader, id)){
  			LOG_ERROR (shader.c_str () << " could not be initialized");
        return false;
  		}
  		glAttachShader (_id, id);
  		if ((error = glGetError ()) != GL_NO_ERROR){
  			LOG_ERROR ("GL error: " << error);
  			return false;
  		}
    }
#endif
    // link program and end
    glLinkProgram (_id);
    if ((error = glGetError ()) != GL_NO_ERROR){
    	LOG_ERROR ("GL error: " << error);
      return false;
    }
    return true;
	}

	bool GLProgram::Unload ()
	{
		GLenum error;

    if (_id){
      glDeleteProgram (_id);
      if ((error = glGetError ()) != GL_NO_ERROR){
      	LOG_ERROR ("GL error: " << error);
        return false;
      }
    }
		return true;
	}

	bool GLProgram::LoadShader (const GLenum type, const string& file, GLuint& id)
	{
    GLenum error;

    id = glCreateShader (type);
    if ((error = glGetError ()) != GL_NO_ERROR){
    	LOG_ERROR ("GL error: " << error);
      return false;
    }

    ifstream is;
    is.open (file.c_str (), ios::binary);
    if (is.fail ()){
    	LOG_ERROR ("Could not open shader file " << file);
    }
    is.seekg (0, ios::end);
    size_t length = is.tellg ();
    is.seekg (0, ios::beg);
    char *src = new char [length + 1];
    is.read (src, length);
    is.close ();
    src [length] = '\0';

    string code (src);
    delete [] src;

    const char *source = code.c_str ();

    glShaderSource (id, 1, &source, NULL);
    if ((error = glGetError ()) != GL_NO_ERROR){
    	LOG_ERROR ("GL error: " << error);
      return false;
    }

    glCompileShader (id);
    if ((error = glGetError ()) != GL_NO_ERROR){
    	LOG_ERROR ("GL error: " << error);
      return false;
    }

    GLint compileStatus;
    glGetShaderiv (id, GL_COMPILE_STATUS, &compileStatus);
    if ((error = glGetError ()) != GL_NO_ERROR || compileStatus == GL_FALSE){

      char log [1024];
      GLsizei loglength;
      glGetShaderInfoLog (id, 1024, &loglength, log);
      LOG_ERROR ("GL error: Shader Info Log [" << file.c_str () << "]:\n" << log);
      return false;
    }
    return true;
	}
}

