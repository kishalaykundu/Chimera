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
#include <memory>

#include "Preprocess.h"
#include "Vector.h"

using std::shared_ptr;

namespace Sim {
	class MeshLoader {

	public:
		// triple-buffered vertex buffer loading function
		static bool LoadVertices (const char* file, unsigned int& numVerts, shared_ptr <Vector>& vertices)
		{
			FILE* fp = fopen (file, "r");
			if (fp == nullptr){
				LOG_ERROR ("Could not open " << file);
				return false;
			}

			int nVerts = 0;
			int status = fscanf (fp, "%d\n", &nVerts);
			if (!status || numVerts <= 0){
				LOG_ERROR ("Invalid vertex count" << nVerts << " specified in " << file);
				fclose (fp);
				return false;
			}
			numVerts = static_cast <unsigned int> (nVerts);
			vertices = shared_ptr <Vector> (new Vector [3*numVerts], DeleteArray <Vector> ());
			if (!vertices){
				LOG_ERROR ("Could not allocate 1st vertex array of size " << numVerts << " from " << file);
				return false;
			}

			Vector* vptr = vertices.get ();
			Real verts [SIM_VECTOR_SIZE];
#			ifdef SIM_VECTOR4_ENABLED
			verts [3] = 1.;
#			endif
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
				for (unsigned int j = 0; j < SIM_VECTOR_SIZE; ++j){
					(vptr [i])[j] = verts [j];
				}
			}
			fclose (fp);

			// populate 2nd buffer
			unsigned int offset = numVerts;
			for (unsigned int i = 0; i < numVerts; ++i){
				vptr [offset + i] = vptr [i];
			}

			// populate 3rd buffer
			offset *= 2;
			for (unsigned int i = 0; i < numVerts; ++i){
				vptr [offset + i] = vptr [i];
			}

			return true;
		}

		// function to get number of elements in an index file
		static unsigned int GetIndexCount(const char* file)
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

		// function to load indices from file (assumes pre-allocated index array)
		static bool LoadIndices (const char* file, unsigned int n, unsigned int* indices)
		{
			// sanity check
			if (n < 2 || n > 4){
				LOG_ERROR ("Invalid cardinality " << n << " specified for index file " << file);
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

			switch (n){
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
