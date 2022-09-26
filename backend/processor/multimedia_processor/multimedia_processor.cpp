// multimedia_processor.cpp : Defines the entry point for the application.
//

#include "multimedia_processor.h"
#include <pmp/SurfaceMesh.h>

using namespace std;

int main(int argc, char *argv[])
{
	if (argc < 2)
		printf("Please supply an argument");
	// No switch for "strings" :(
	else if (!strcmp(argv[1], "debug"))
		debug();
	else
		printf("Unknown argument");

	return 0;
}

void debug()
{
	pmp::SurfaceMesh mesh;
	mesh.read("../../frontend/model-viewer/public/models/animal/m0.obj");

	printf("<br> Waddup ik ben een c++ skrippie! <br> #vertices: %zu, #faces: %zu ", mesh.n_vertices(), mesh.n_faces());
}
