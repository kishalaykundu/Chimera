/**
 * @file PNGUtils.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See PNGUtils.h.
 */
#include <vector>

#include "Preprocess.h"
#include "Log.h"
#include "PNGUtils.h"

using std::vector;

namespace Sim {

	bool PNGDecoder::operator () (unsigned char* data)
	{
		// purge any existing data
		SAFE_DELETE (data);

		vector <unsigned char> image; //the raw pixels

		// decode
		auto error = lodepng::decode(image, _width, _height, _file);
		if (error){
			LOG_ERROR ("Could not load " << _file << ". Error no. " << error << ": " << lodepng_error_text (error));
			data = nullptr;
			return false;
		}
		data = image.data ();

		return true;
	}

	bool DecodePNG (const char* file, unsigned int width, unsigned int height, unsigned char* data)
	{
		vector <unsigned char> image; //the raw pixels

		// decode
		unsigned int error = lodepng::decode(image, width, height, file);
		if (error){
			LOG_ERROR ("Could not load " << file << ". Error no. " << error << ": " << lodepng_error_text(error));
			data = nullptr;
			return false;
		}
		data = image.data ();

		return true;
	}

	bool EncodePNG (const char* file, unsigned int width, unsigned int height, unsigned char* data)
	{
		// form a vector from array - lodepng deals with vectors
		unsigned int size = sizeof (unsigned char)*4*width*height;
		vector <unsigned char> image (data, data + size);

		// encode
		unsigned int error = lodepng::encode (file, image, width, height);
		if (error){
			LOG_ERROR ("Could not write to " << file << ". Error no. " << error << ": " << lodepng_error_text(error));
			delete [] data;
			return false;
		}
		delete [] data;

		return true;
	}
}
