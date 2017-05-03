/**
 * @file main.cpp
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The entry point for the Chimera simulation framework. This is where
 * everything starts. Here, the driver, the simulation core logic and
 * user interfaces are initialized and synchronized first. The program
 * then goes on to rest in the simulation-loop and at the end, finally
 * exits cleanly.
 */
#include <cstdlib>
#include <cstring>

#include "Preprocess.h"
#include "GLDriver/Driver.h"

using Sim::Driver;

int main (int argc, const char** argv)
{
	// sanity check
		if (argc == 2) {
			if (!strcmp (argv [1], "-h") || !strcmp (argv [1], "--help")){
				LOG ("Usage: ./Bin/simulate <config file> (default: Assets/Config/ChimeraConfig.xml)");
				exit (EXIT_SUCCESS);
			}
		}

		// READ INPUT FILE AND INITIALIZE
		{
			const char* input = nullptr;
			if (argc < 2){
				input = "Assets/Config/ChimeraConfig.xml";
			} else {
				input = argv [1];
			}
			LOG ("Reading " << input << "...");
			if (!Driver::Instance ().Initialize (input)){
				LOG_ERROR ("Fatal error: Application failed to start. Aborting..");

				Driver::Instance() .Quit ();
				exit (EXIT_FAILURE);
			}
		}

		// THE MAIN LOOP
		Driver::Instance ().Run ();

		// PREPARE FOR EXITING: CLEAN ALL APP RESOURCES
		Driver::Instance ().Cleanup ();

	exit (EXIT_SUCCESS);
}
