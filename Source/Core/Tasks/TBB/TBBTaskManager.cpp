/**
 * @file TBBTaskManager.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See TBBTaskManager.h.
 */

#include "Preprocess.h"
#include "Tasks/TBB/TBBTaskManager.h"

namespace Sim {

	TBBTaskManager::TBBTaskManager ()
	{
		LOG ("TBB task manager constructed");
	}

	TBBTaskManager::~TBBTaskManager ()
	{
		Cleanup ();
		LOG ("TBB task manager destroyed");
	}

	TBBTaskManager::TBBTaskManager (const TBBTaskManager& tm) {}

	TBBTaskManager& TBBTaskManager::operator = (const TBBTaskManager& tm) {return *this;}

	bool TBBTaskManager::Initialize (const char* configfile)
	{
		return true;
	}

	void TBBTaskManager::Cleanup ()
	{

	}
}
