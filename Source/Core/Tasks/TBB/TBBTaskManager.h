/**
 * @file TBBTaskManager.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The Intel TBB based task manager.
 */
#pragma once

#include "Tasks/TaskManager.h"

namespace Sim {

	class TBBTaskManager : public TaskManager {

		private: // forbidden copy constructor and assignment operator
			TBBTaskManager (const TBBTaskManager&);
			TBBTaskManager& operator = (const TBBTaskManager&);

		public:
			TBBTaskManager ();
			~TBBTaskManager ();

			virtual bool Initialize (const char* config) override;
			virtual void Cleanup () override;
	};
}
