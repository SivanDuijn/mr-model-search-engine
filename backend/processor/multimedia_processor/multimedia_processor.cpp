// multimedia_processor.cpp : Defines the entry point for the application.
//

#include "multimedia_processor.h"
#include <pmp/SurfaceMesh.h>

using namespace std;

int main(int argc, char *argv[])
{
	pmp::SurfaceMesh mesh;
	mesh.read("../../frontend/model-viewer/public/models/animal/m0.obj");
	cout << "# vertices: " << mesh.n_vertices() << endl;
	cout << "# faces: " << mesh.n_faces() << endl;

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
	printf("Waddup ik ben een c++ skrippie");
}
