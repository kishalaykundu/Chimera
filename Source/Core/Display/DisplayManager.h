/**
 * @file DisplayManager.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The interface for the display managing system for Chimera. It
 * is in charge of any hardware-accelerated graphics related
 * algorithms run by the system. Some examples are OpenGL, Vulkan
 * etc.
 */
#pragma once

namespace Sim {

	class DisplayManager {

		public:
			DisplayManager () {}
			virtual ~DisplayManager () {}

			virtual bool Initialize (const char* config) = 0;
			virtual void Cleanup () = 0;
	};
}
