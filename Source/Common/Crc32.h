/**
 * @file Crc32.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * A simple crc32 hash code calculating program implemented in
 * C++ format.
 */
#pragma once

namespace Sim {

	class Crc32 {
	private:
		unsigned int _table [256];
	public:
		Crc32 ();

		unsigned int CalculateHash (const char* message);
	};
}
