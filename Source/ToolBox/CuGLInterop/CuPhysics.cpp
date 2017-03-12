/*
 * CuPhysics.cpp
 *
 *  Created on: 08-Mar-2017
 *      Author: kishalay
 */

#include <iostream>

#include "GLDisplay.h"
#include "CuPhysics.h"

using std::cerr;
using std::endl;
using std::shared_ptr;

namespace Sim {

	bool CuPhysics::Initialize (shared_ptr <Sim::GLDisplay>& d)
	{
		_display = d->Display ();
		_context = glXCreateContextAttribsARB (_display, d->Config (), d->Context (), true, d->ContextAttributes ());
		if (!_context){
			cerr << "Could not create GLX context" << endl;
		  return false;
		}

		_runFlag = true;
		return true;
	}

	void CuPhysics::Run ()
	{
		glXMakeContextCurrent (_display, NULL, NULL, _context);

		while (_runFlag){

		}
	}

	void CuPhysics::Cleanup ()
	{
 		glXDestroyContext (_display, _context);
	}
}
