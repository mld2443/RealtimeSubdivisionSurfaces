////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include <sstream>
#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	std::stringstream stream;
	std::string arg;
	SystemClass* System;
	float tessFactor, x0, y0, z0, x1, y1, z1, x2, y2, z2;
	bool result;

	// Set default input values
	tessFactor = 33.0f;
	x0 = 3.0f, y0 = 2.0f, z0 = 0.0f;
	x1 = -0.5f, y1 = 0.5f, z1 = 0.0f;
	x2 = 3.0f, y2 = 2.0f, z2 = 0.0f;
	
	// Take in the command line arguments
	stream << pScmdline;

	// Parse the arguments
	while (stream >> arg) {
		if (arg[0] != '-' || ((arg[1] == 'p' || arg[1] == 'P') && arg.length() != 3) || ((arg[1] != 'p' && arg[1] != 'P') && arg.length() != 2)) {
			printf("Incorrect arguments: \"%s\"\nTry \'-h\' for help", arg.data());
			return 1;
		}

		switch (arg[1])
		{
		case 'h':
		case 'H':
			printf("-t [tessFactor] -c [camera x y z] -p1 [left x y z] -p2 [right x y z]\n");
			break;

		case 't':
		case 'T':
			stream >> tessFactor;
			break;

		case 'c':
		case 'C':
			stream >> x0 >> y0 >> z0;
			break;

		case 'p':
		case 'P':
			switch (arg[2])
			{
			case '1':
				stream >> x1 >> y1 >> z1;
				break;

			case '2':
				stream >> x2 >> y2 >> z2;
				break;

			default:
				printf("Incorrect arguments: \"%s\"\nTry \'-h\' for help", arg.data());
				return 1;
				break;
			}
			break;

		default:
			printf("Incorrect arguments: \"%s\"\nTry \'-h\' for help", arg.data());
			return 1;
			break;
		}
	}
	
	// Create the system object.
	System = new SystemClass;
	if(!System)
	{
		return 2;
	}

	// Initialize and run the system object.
	result = System->Initialize(tessFactor);
	if(result)
	{
		System->Run();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}