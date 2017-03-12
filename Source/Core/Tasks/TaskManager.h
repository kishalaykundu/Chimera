/**
 * @file TaskManager.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The manager for the task/thread pool. This is the scheduler for all
 * threaded tasks. This is an interface which is specialized by a more
 * platform-specific task scheduler like Intel TBB or threadpool
 */
#pragma once

namespace Sim {

	class TaskManager {

		protected: // forbidden copy constructor and assignment operator
			TaskManager (const TaskManager& t) {}
			TaskManager& operator = (const TaskManager& t) {return *this;}

		public:
			TaskManager () {}
			virtual ~TaskManager () {}

			virtual bool Initialize (const char* config) {return true;}
			virtual void Update () {}
			virtual void Cleanup () {}
	};
}
