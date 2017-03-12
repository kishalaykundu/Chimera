/**
 * @file UIDGenerator.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 * See UIDGenerator.h.
 */

#include <climits>
#include <mutex>

#include "UIDGenerator.h"

using std::mutex;

namespace Sim {

	UIDGenerator* UIDGenerator::_instance = nullptr;

	static std::random_device _dev;
	static std::mt19937 _mers (_dev ());
	static std::uniform_int_distribution <int> _dist (1, USHRT_MAX);

	unsigned int UIDGenerator::GetUniqueId ()
	{
		_mutex.lock ();
		return _dist (_mers);
		_mutex.unlock ();
	}
}
