/**
 * @file SharedLib.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The generic shared library class used by Chimera. The class is used
 * to load dynamically linked plugin libraries and their symbols.
 */
#pragma once

#include <string>
extern "C" {
#include <dlfcn.h>
}
#if defined (__linux__) || defined (MACOSX)
#	define SIM_LIB_HANDLE void*
#endif

namespace Sim {

	class SharedLib {

	protected:
		SIM_LIB_HANDLE _handle;

	private:
		// forbidden copy constructor and assignment operator
		SharedLib (const SharedLib &p) : _handle (p._handle) {}
		SharedLib& operator = (const SharedLib& p) {_handle = p._handle; return *this;}

	public:
		SharedLib (): _handle (nullptr) {LOG ("Empty shared lib constructed");}
		~SharedLib () {LOG ("Shared lib destroyed");}

		// function to dynamically load library
		bool Load (const std::string& name)
		{
			_handle = dlopen (name.c_str (), RTLD_LAZY);
			if (_handle == nullptr){
				LOG (name.c_str () << "could not be loaded\n[" << dlerror () << "]");
				return false;
			}
			return true;
		}
		// function to unload library
		bool Unload (const std::string& name)
		{
			if (_handle == nullptr){
				return true;
			}
			if (dlclose (_handle)){
				LOG ("error: " << name.c_str () << " could not be unloaded [" << dlerror () << "]");
				return false;
			}
			return true;
		}
		// function to return given symbol by name
		void* GetSymbol (const char* name) const
		{
			return reinterpret_cast <void*> (dlsym (_handle, name));
		}
	};
}
