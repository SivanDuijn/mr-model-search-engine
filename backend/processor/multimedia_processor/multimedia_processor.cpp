// multimedia_processor.cpp : Defines the entry point for the application.
//

#include "multimedia_processor.h"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2) printf("Please supply an argument");
	// No switch for "strings" :(
	else if (!strcmp(argv[1], "debug")) debug();
	else printf("Unknown argument");

	return 0;
}

void debug()
{
	printf("Echoing debug call");
}
