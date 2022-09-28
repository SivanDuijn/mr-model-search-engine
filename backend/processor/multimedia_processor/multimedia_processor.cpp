#include "multimedia_processor.h"

using namespace std;

int main(int argc, char *argv[])
{
	normalize();
	if (argc < 2) printf("Please supply an argument");
	// No switch for "strings" :(
	else if (!strcmp(argv[1], "debug"))     debug();
	else if (!strcmp(argv[1], "resample"))  resample();
	else if (!strcmp(argv[1], "normalize")) normalize();
	else printf("Unknown argument");

	return 0;
}

void debug()
{
	printf("Echoing debug call\n");
}

// Resample a mesh at the given path
void resample(const char* path)
{
	// Get the mesh
	printf("Loading mesh \"%s\"\n", path);
	pmp::SurfaceMesh mesh;
	mesh.read(vars::GetAssetPath(path));

	printf("Mesh has %zu vertices\n", mesh.n_vertices());

	// Triangulate it if necessary
	if (!mesh.is_triangle_mesh())
	{
		printf("Triangulating mesh");
		pmp::Triangulation tri = pmp::Triangulation(mesh);
		tri.triangulate();
	}

	// TODO find actual desired vertex count
	const unsigned int des = 5000;

	// Subdivide and/or decimate depending on the vertex count
	if (mesh.n_vertices() < des)
	{
		printf("Subdividing mesh");
		pmp::Subdivision div = pmp::Subdivision(mesh);
		div.catmull_clark();
		printf(" to %zu vertices\n", mesh.n_vertices());
	}
	if (mesh.n_vertices() > des)
	{
		printf("Decimating mesh");
		pmp::Decimation dec = pmp::Decimation(mesh);
		dec.decimate(des);
		printf(" to %zu vertices\n", mesh.n_vertices());
	}
	else
	{
		printf("Mesh contains exactly the right amount of vertices!\n");
	}

	// Write resampled mesh to disk
	// TODO file name/location
	printf("Writing mesh to disk\n");
	mesh.write(vars::GetAssetPath(path) + ".res.off");

	printf("Done\n");
}

// Normalize a mesh according to 4 steps:
// * Translate to barycenter
// * Align with coordinate frame
// * Flip based on moment test
// * Scale to unit volume
void normalize(const char* path)
{
	// Get the mesh
	printf("Loading mesh \"%s\"\n", path);
	pmp::SurfaceMesh mesh;
	mesh.read(vars::GetAssetPath(path));

	// Calculate the barycenter of the mesh
	printf("Translating mesh to barycenter\n");
	pmp::vec3 bcenter = vreduce
	(
		mesh,
		[](pmp::Vertex v)->pmp::vec3 
		{
			return pmp::vec3(0); 
		},
		[](pmp::vec3 a, pmp::vec3 b)-> pmp::vec3
		{ 
			return a + b;
		},
		pmp::vec3(0)
	);
	map
	(
		mesh,
		[](pmp::Vertex) { }
	);

	printf("Center at [%f, %f, %f]", bcenter.data()[0], bcenter.data()[1], bcenter.data()[2]);
	throw exception("Not yet implemented");
}

// Apply a function to all vertices of a mesh
void map(pmp::SurfaceMesh &mesh, void (*fnc) (pmp::Vertex))
{
	pmp::SurfaceMesh::VertexIterator i;
	for (i = mesh.vertices_begin(); i != mesh.vertices_end(); i++)
	{
		fnc(*i);
	}
}

// Reduce all vertices of a mesh to a vector
pmp::vec3 vreduce(pmp::SurfaceMesh& mesh, pmp::vec3(*cnv) (pmp::Vertex), pmp::vec3(*red) (pmp::vec3, pmp::vec3), pmp::vec3 bse)
{
	pmp::SurfaceMesh::VertexIterator i;
	for (i = mesh.vertices_begin(); i != mesh.vertices_end(); i++)
	{
		bse = red(bse, cnv(*i));
	}
	return bse;
}
