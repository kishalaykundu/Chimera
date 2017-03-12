/**
 * @file Preprocess.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * Common definitions prerequisites for the Chimera Framework.
 */
#pragma once

#include "Config.h"

// define flag based LOG functionality
#ifdef SIM_LOG_ENABLED
#	include <cstring> // basename (...) is defined in cstring
#	include <iostream>
#	ifndef NDEBUG // print debug message, prefixed by the file and line number
#		define LOG(x) (std::cout << basename( __FILE__ ) << "[" << __LINE__ << "]: " << x << "." << std::endl)
#		define LOG_ERROR(x) (std::cerr << basename( __FILE__ ) << "[" << __LINE__ << "]: error: " << x << "." << std::endl)
#		define LOG_WARNING(x) (std::cout << basename( __FILE__ ) << "[" << __LINE__ << "]: warning: " << x << "." << std::endl)
#	else // print normal style string
#		define LOG(x) (std::cout << x << "." << std::endl)
#		define LOG_ERROR(x) (std::cerr << "error: " << x << "." <<std::endl)
#		define LOG_WARNING(x) (std::cout << "warning: " << x << "." << std::endl)
#	endif
#else
#	define LOG(x) // no messages
#	define LOG_ERROR(x) // no messages
#	define LOG_WARNING(x) // no messages
#endif

namespace Sim {

#define SIM_ENDIAN_LITTLE 1
#define SIM_ENDIAN_BIG 2

#define SIM_ARCH_32 1
#define SIM_ARCH_64 2

	// set precision
#ifdef SIM_DOUBLE_PRECISION
	typedef double Real;
#	define EPSILON 1e-9
#else
	typedef float Real;
#	define EPSILON 1e-6
#endif

	// array deleter normally used by shared_ptr to arrays
	template <typename T> class DeleteArray { public: void operator () (T const *p) {delete [] p;} };

	inline void SafeDelete (auto x) { if (x != nullptr) {delete x; x = nullptr;} }
	inline auto Absolute (auto f) {return f > 0. ? f : -f;}
	inline int Sign (auto x) {return x > 0. ? 1 : -1;}
	inline auto Max (auto x, auto y) {return x > y ? x : y;}

#define EXPORT __attribute__((visibility("default")))
#define HIDDEN __attribute__((visibility("hidden")))
}
