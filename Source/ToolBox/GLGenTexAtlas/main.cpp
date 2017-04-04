/**
 * @file main.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * Tool to convert 3D Chimera-style mesh files with corresponding 3D
 * textures to their texture atlas. This tool can handle atlas gene-
 * ration for both mass-spring as well as finite element meshes. The
 * mass spring conversion results in 2D texture atlas and the corre-
 * sponding texture coordinates. The finite element converstion gen-
 * erates a 2D texture atlas of texture coordinates  for the surface
 * of the model as well as texture coordinates for all the vertices.
 */

#include <cstdlib>
#include <cctype>

#include "tinyxml2.h"
#include "Preprocess.h"
#include "InputParser.h"
#include "GLMesh.h"

using tinyxml2::XMLElement;
using tinyxml2::XMLError;
using tinyxml2::XML_SUCCESS;
using Sim::InputParser;

float aspect [] = {1., 1., 1.};
unsigned int depth = 0;

char* type = nullptr;
const char* inFilename = nullptr;
const char* inLocation = nullptr;
const char* inTexFilename = nullptr;
const char* outTexFilename = nullptr;
const char* texCoordFilename = nullptr;

bool ReadInput (const char* config)
{
	InputParser p;
	if (!p.Initialize (config, "AtlasConfig")){
		LOG_ERROR ("Could not initialize parser for " << config);
		return false;
	}

	XMLElement* elem = p.GetElement ("Geometry");
	if (elem == nullptr){
		LOG_ERROR ("No \'Geometry\' specified by " << config);
		return false;
	}

	const char* inputtype = elem->Attribute ("Type");
	if (inputtype == nullptr){
		LOG_ERROR ("No type specified by " << config);
		return false;
	}

	// convert type string to all upper
	{
		unsigned int size = strlen (inputtype);
		type = new char [size + 1];
		type [size] = '\0';
		for (unsigned int i = 0; i < size; ++i){
			if (isupper (inputtype [i])){
				type [i] = tolower (inputtype [i]);
			} else {
				type [i] = inputtype [i];
			}
		}
	}
	if (strcmp ("msd", type) || strcmp ("fem", type)){
		LOG_ERROR ("Invalid type: " << type << ". Should be FEM or MSD");
		return false;
	}

	// get the number of index files
	XMLError error = elem->QueryUnsignedAttribute ("Depth", &depth);
	if (error != XML_SUCCESS){
		LOG_ERROR ("Invalid \'Depth\' specified in " << config);
		return false;
	}

	inFilename = elem->Attribute ("Prefix");
	if (inFilename == nullptr){
		LOG_ERROR ("No Geometry file prefix specified in " << config);
		return false;
	}

	inLocation = elem->Attribute ("Location");
	if (inLocation == nullptr){
		LOG_ERROR ("No Geometry location specified in " << config);
		return false;
	}

	XMLElement* celem = elem->FirstChildElement ("Texture");
	if (celem == nullptr){
		LOG_ERROR ("No input Texture specified in " << config);
		return false;
	}
	celem = celem->FirstChildElement ("Location");
	if (celem == nullptr){
		LOG_ERROR ("No input Texture file location specified in " << config);
		return false;
	}
	inTexFilename = celem->Value ();

	celem = celem->NextSiblingElement ("AspectRatio");
	if (celem != nullptr){
		error = celem->QueryFloatAttribute ("X", &(aspect [0]));
		if (error != XML_SUCCESS){
			LOG_ERROR ("Invalid x-component for aspect ratio specified in " << config);
			return false;
		}
		error = celem->QueryFloatAttribute ("Y", &(aspect [1]));
		if (error != XML_SUCCESS){
			LOG_ERROR ("Invalid y-component for aspect ratio specified in " << config);
			return false;
		}
		error = celem->QueryFloatAttribute ("Z", &(aspect [2]));
		if (error != XML_SUCCESS){
			LOG_ERROR ("Invalid z-component for aspect ratio specified in " << config);
			return false;
		}
	}

	// get output filenames
	elem = p.GetElement ("Output");
	if (elem == nullptr){
		LOG_ERROR ("No \'Output\' specified in " << config);
		return false;
	}

	celem = elem->FirstChildElement ("Texture");
	if (celem == nullptr){
		LOG_ERROR ("No output \'Texture\' file name specified in " << config);
		return false;
	}
	outTexFilename = celem->Value ();

	celem = elem->FirstChildElement ("TextureCoordinates");
	if (celem == nullptr){
		LOG_ERROR ("No \'TextureCoordinates\' output file name specified in " << config);
		return false;
	}
	texCoordFilename = celem->Value ();

	return true;
}

int main (int argc, const char** argv)
{
	// sanity check
	if (argc < 2){
		LOG_ERROR ("Usage: ./Bin/glGenAtlas <xml-configuration-file>");
		exit (EXIT_FAILURE);
	}
	else if (!strcmp ("-h", argv [1]) || !strcmp ("--help", argv [1])){
		LOG ("Usage: ./Bin/glGenAtlas <xml-configuration-file>");
		exit (EXIT_SUCCESS);
	}

	// read input configuration file
	if (!ReadInput (argv [1])){
		LOG_ERROR ("Could not read input configuration file");
		exit (EXIT_FAILURE);
	}

	Sim::GLMesh mesh;
	if (!mesh.Initialize (depth, inLocation, inFilename)){
		LOG_ERROR ("Mesh files could not be loaded");
		exit (EXIT_FAILURE);
	}

	exit (EXIT_SUCCESS);
}
