/**
 * @file PNGUtils.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * Functions to read and write PNG image files and return the raw
 * data.
 */

#include "lodepng.h"

namespace Sim {

	class PNGDecoder {

	private:
		const char* _file = nullptr;
		unsigned int _width = 0, _height = 0;

	public:
		PNGDecoder (const char* file, unsigned int width, unsigned int height)
		: _file (file), _width (width), _height (height) {}
		~PNGDecoder () = default;

		// forbidden constructors and operators
		PNGDecoder () = delete;
		PNGDecoder (const PNGDecoder&) = delete;
		PNGDecoder& operator = (const PNGDecoder&) = delete;

		// functor function
		bool operator () (unsigned char* data);
	};

	bool DecodePNG (const char* file, unsigned int width, unsigned int height, unsigned char* data);

	bool EncodePNG (const char* file, unsigned int width, unsigned int height, unsigned char* data);
}
