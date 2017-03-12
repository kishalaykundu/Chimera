/**
 * @file Driver.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The Driver interface to be used by all non-engine code files. This
 * provides up with an easy interface to the platform/software specific
 * implementation of the central driver application.
 */
#pragma once

#include "Config.h"

#if defined (SIM_GL_ENABLED) && defined (SIM_CUDA_ENABLED) && defined (SIM_TBB_ENABLED)
#	include "GL-CUDA-TBB/Driver.h"
#endif
