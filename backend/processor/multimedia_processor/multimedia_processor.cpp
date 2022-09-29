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

	// Example desired vertex count mentioned in technical tips
	const unsigned int des = 1000;

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
// * Translate barycenter to origin
// * Align with coordinate frame
// * Flip based on moment test
// * Scale to unit volume
void normalize(const char* path)
{
	// Get the mesh
	printf("Loading mesh \"%s\"\n", path);
	pmp::SurfaceMesh mesh;
	mesh.read(vars::GetAssetPath(path));

	// Translate barycenter to origin
	printf("Translating barycenter to origin");
	pmp::VertexProperty points = mesh.get_vertex_property<pmp::Point>("v:point");
	pmp::Point bcenter = pmp::centroid(mesh);
	for (auto v : mesh.vertices()) points[v] -= bcenter;
	printf(" (-[%f, %f, %f])\n", bcenter.data()[0], bcenter.data()[1], bcenter.data()[2]);

	// Align with coordinate frame
	printf("Aligning with coordinate frame");
	pmp::mat3 test = pmp::mat3(pmp::vec3(134, -18, -116), pmp::vec3(-18, 36, -18), pmp::vec3(-116, -18, 134));
	Eigen::EigenSolver<Eigen::MatrixXf> eigen;
	eigen.compute((Eigen::Matrix3f) test);
	Eigen::MatrixXf eigen_vectors = eigen.eigenvectors().real();
	printf(" ([%f, %f, %f] / [%f, %f, %f])\n", eigen_vectors(0, 0), eigen_vectors(0, 1), eigen_vectors(0, 2), eigen_vectors(1, 0), eigen_vectors(1, 1), eigen_vectors(1, 2));

	// TODO
	throw exception("Not yet implemented");
}
