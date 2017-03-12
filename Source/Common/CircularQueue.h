/**
 * @file CircularQueue.h
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
 * Template for thread-safe circular queue.
 */
#pragma once

#include <mutex>
#include <condition_variable>

namespace Sim {

	template <class T, unsigned int size> class CircularQueue {

		private:
			std::mutex _mutex;
			std::condition_variable _full, _empty;

			unsigned int _readIndex;
			unsigned int _writeIndex;
			T _queue [size];

		public:
			CircularQueue (): _readIndex (0), _writeIndex (0) {}
			~CircularQueue () {}
			void Push (const T& e)
			{
				std::unique_lock <std::mutex> loki (_mutex);
				while (Full ()) {
					_full.wait (loki);
				}

				_queue [Index (_writeIndex)] = e;
				_writeIndex = Index (_writeIndex + 1);

				_empty.notify_one ();
			}

			void Pop (T& result)
			{
				std::unique_lock <std::mutex> loki (_mutex);
				while (Empty ()) {
					_empty.wait (loki);
				}

				result = _queue [Index (_readIndex)];
				_readIndex = Index (_readIndex + 1);

				_full.notify_one ();
			}

		protected:
			bool Full () {return (_writeIndex -_readIndex == size - 1) || (_writeIndex == _readIndex - 1);}
			bool Empty () {return (_writeIndex == _readIndex);}
			unsigned int Index (const unsigned int index) {return index % size;}
	};
}
