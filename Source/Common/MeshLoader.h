/**
 * @file MeshLoader.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * Generalized methods to load geometry info from files.
 */
#pragma once

#include <cstdio>

#include "Preprocess.h"
#include "Vector.h"

namespace Sim {
	namespace MeshLoader {

		// function to get number of elements in an index file
		static unsigned int GetElementCount(const char* file)
		{
			FILE* fp = fopen (file, "r");
			if (fp == nullptr){
				LOG_ERROR ("Could not open " << file);
				return 0;
			}

			int nInds = 0;
			int status = fscanf (fp, "%d\n", &nInds);
			if (!status || nInds <= 0){
				LOG_ERROR ("Invalid index count " << nInds << " specified in " << file);
				fclose (fp);
				return 0;
			}
			fclose (fp);

			return static_cast <unsigned int> (nInds);
		}

		// vertex buffer loading function (pre-allocated array)
		template <int size> bool LoadVertices (const char* file, SimVector<size>* vertices)
		{
			FILE* fp = fopen (file, "r");
			if (fp == nullptr){
				LOG_ERROR ("Could not open " << file);
				return false;
			}

			int nVerts = 0;
			int status = fscanf (fp, "%d\n", &nVerts);
			if (!status || nVerts <= 0){
				LOG_ERROR ("Invalid vertex count" << nVerts << " specified in " << file);
				fclose (fp);
				return false;
			}
			unsigned int numVerts = static_cast <unsigned int> (nVerts);

			Real verts [SIM_VECTOR_SIZE];
#			ifdef SIM_VECTOR4_ENABLED
			verts [3] = 1.;
#			endif
			if (size > 2){
				for (unsigned int i = 0; i < numVerts; ++i){

	#			ifdef SIM_DOUBLE_PRECISION
					status = fscanf (fp, "%lf %lf %lf\n", &(verts [0]), &(verts [1]), &(verts [2]));
	#			else
					status = fscanf (fp, "%f %f %f\n", &(verts [0]), &(verts [1]), &(verts [2]));
	#			endif
	#			ifndef NDEBUG
					if (!status){
						LOG_ERROR ("Could not read vertex no. " << i << " in " << file);
						fclose (fp);
						return false;
					}
	#			endif
					for (unsigned int j = 0; j < size; ++j){
						vertices [i][j] = verts [j];
					}
				}
			}
			else {
				for (unsigned int i = 0; i < numVerts; ++i){

	#			ifdef SIM_DOUBLE_PRECISION
					status = fscanf (fp, "%lf %lf\n", &(verts [0]), &(verts [1]));
	#			else
					status = fscanf (fp, "%f %f\n", &(verts [0]), &(verts [1]));
	#			endif
	#			ifndef NDEBUG
					if (!status){
						LOG_ERROR ("Could not read vertex no. " << i << " in " << file);
						fclose (fp);
						return false;
					}
	#			endif
					for (unsigned int j = 0; j < size; ++j){
						vertices [i][j] = verts [j];
					}
				}
			}
			fclose (fp);
			return true;
		}

		// function to load indices from file (pre-allocated array)
		template <int size> bool LoadIndices (const char* file, unsigned int* indices)
		{
			// sanity check
			if (size < 2 || size > 4){
				LOG_ERROR ("Invalid cardinality " << size << " specified for index file " << file);
				return false;
			}

			FILE* fp = fopen (file, "r");
			if (fp == nullptr){
				LOG_ERROR ("Could not open " << file);
				return false;
			}

			int nInds = 0;
			int status = fscanf (fp, "%d\n", &nInds);
			if (!status || nInds <= 0){
				LOG_ERROR ("Invalid index count " << nInds << " specified in " << file);
				fclose (fp);
				return false;
			}
			unsigned int numIndices = static_cast <unsigned int> (nInds);

			int inds [4];
			inds [0] = 0; inds [1] = 0; inds [2] = 0; inds [3] = 0;

			switch (size){
			case 2:
				for (unsigned int i = 0; i < numIndices; ++i){
					status = fscanf ( fp, "%d %d\n", &(inds [0]), &(inds [1]));
#					ifndef NDEBUG
					if (!status || inds [0] < 0 || inds [1] < 0){
						LOG_ERROR ("Invalid index set [" << i << "]: " << inds [0] << ", " << inds [1] << " in " << file);
						fclose (fp);
						return false;
					}
#					endif
					for (unsigned int j = 0; j < 2; ++j){
						indices [2*i + j] = static_cast <unsigned int> (inds [j]);
					}
				}
				break;
			case 3:
				for (unsigned int i = 0; i < numIndices; ++i){
					status = fscanf ( fp, "%d %d %d\n", &(inds [0]), &(inds [1]), &(inds [2]));
#					ifndef NDEBUG
					if (!status || inds [0] < 0 || inds [1] < 0 || inds [2] << 0 || inds [3] < 0){
						LOG_ERROR ("Invalid index set [" << i << "]: " << inds [0] << ", " << inds [1] << ", " << inds [2] << " in " << file);
						fclose (fp);
						return false;
					}
#					endif
					for (unsigned int j = 0; j < 3; ++j){
						indices [3*i + j] = static_cast <unsigned int> (inds [j]);
					}
				}
				break;
			case 4:
				for (unsigned int i = 0; i < numIndices; ++i){
					status = fscanf ( fp, "%d %d %d %d\n", &(inds [0]), &(inds [1]), &(inds [2]), &(inds [3]));
#					ifndef NDEBUG
					if (!status || inds [0] < 0 || inds [1] < 0 || inds [2] << 0 || inds [3] < 0){
						LOG_ERROR ("Invalid index set [" << i << "]: " << inds [0] << ", " << inds [1] <<
								", " << inds [2] << ", " << inds [3] << " in " << file);
						fclose (fp);
						return false;
					}
#					endif
					for (unsigned int j = 0; j < 4; ++j){
						indices [4*i + j] = static_cast <unsigned int> (inds [j]);
					}
				}
				break;
			default:
				break;
			}
			fclose (fp);

			return true;
		}
	};
}
