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

	bool DecodePNG (const char* file, unsigned int width, unsigned int height, unsigned char* data);

	bool EncodePNG (const char* file, unsigned int width, unsigned int height, unsigned char* data);
}
