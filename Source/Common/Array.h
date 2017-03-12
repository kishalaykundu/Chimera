/**
 * @file Array.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * A templetized wrapper around the C++11 array container.
 */
#pragma once

#include <array>

#include "Preprocess.h"

namespace Sim {
	template <class T, std::size_t max> class Array : public std::array <T, max> {

		private:
			std::size_t _count;

		public:
			Array (): std::array <T, max> (), _count (0) {}
			Array (const Array& arr): std::array <T, max> (arr), _count (arr._count) {}
			Array& operator = (const Array& arr)
			{
				std::array<T, max>::operator = (arr);
				_count = arr._count;
				return *this;
			}

			T& operator [] (std::size_t index)
			{
#				ifndef NDEBUG
				if (index >= _count){
					LOG_WARNING ("Returning bogus element. Please use push_back to add new elements");
				}
#				endif
				return std::array <T, max>::operator [] (index);
			}

			constexpr std::size_t size () const {return _count;}

			inline void push_back (const T& e)
			{
#				ifndef NDEBUG
				if (_count < max) {
#				endif
					std::array <T, max>::operator [] (_count) = e;
					++_count;
#				ifndef NDEBUG
				}
				else {
					LOG_ERROR ("Array is already full. Could not add more elements");
				}
#				endif
			}

			inline void erase (std::size_t index)
			{
#				ifndef NDEBUG
				if (index >= _count){
					LOG_ERROR ("Out of bounds erase specified for Array element");
					return;
				}
#				endif
				LOG_WARNING ("If Array element is of raw pointer type, please delete corresponding object before calling erase");
				for (std::size_t i = index; i < _count - 1; ++i){
					std::array <T, max>::operator [] (i) = std::array <T, max>::operator [] (i + 1);
				}
				--_count;
			}
	};

}
