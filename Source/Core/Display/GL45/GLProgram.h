/**
 * @file GLProgram.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The GPU program class for OpenGL. This class deals with GLSL style
 * programs, loaded through various shaders.
 */
#pragma once

#include <string>
#include "Preprocess.h"
#include "GL/GLUtils.h"

namespace Sim {

	class EXPORT GLProgram {

		protected:
			GLuint _id;
			std::string _name;
			std::string _location;

	public:
		GLProgram (const std::string&, const std::string&);
		~GLProgram ();

		GLuint Id () const { return _id; }
		const std::string& Name () const { return _name; }

		bool Load ();
		bool Unload ();

	protected:
		bool LoadShader (const GLenum, const std::string&, GLuint&);
	};
}
