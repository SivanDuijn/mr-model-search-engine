﻿#include "multimedia_processor.h"

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
		printf("Triangulating mesh\n");
		pmp::Triangulation tri = pmp::Triangulation(mesh);
		tri.triangulate();
	}

	// Example desired vertex count mentioned in technical tips
	const unsigned int des = 2500;

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
	// Map the point list (aka array of Eigen::Matrix3f) to a single Eigen::MatrixXd
	pmp::VertexProperty points = mesh.get_vertex_property<pmp::Point>("v:point");
	Eigen::Map<Eigen::MatrixXf> map((float*)(points.data()), 3, mesh.n_vertices());

	// Translate barycenter to origin
	printf("Translating barycenter to origin");
	pmp::Point bcenter = pmp::centroid(mesh);
	map.colwise() -= (Eigen::Vector3f)bcenter;
	printf(" (-[%f, %f, %f])\n", bcenter.data()[0], bcenter.data()[1], bcenter.data()[2]);

	// Align with coordinate frame
	printf("Aligning with coordinate frame");
	// Compute the covariance matrix
	// https://stackoverflow.com/a/15142446
	// We don't have to center the sample matrix because we just centered the samples :)
	Eigen::Matrix3f cov = (map * map.adjoint()) / float(map.cols() - 1);
	// Get the eigenvectors
	Eigen::EigenSolver<Eigen::MatrixXf> solver;
	solver.compute(cov);
	Eigen::MatrixXf eigen = solver.eigenvectors().real();
	// Rotate the model
	// TODO optimize code
	Eigen::Vector3f	 major = eigen.row(0);
	Eigen::Vector3f minor = eigen.row(1);
	Eigen::Vector3f cross = major.cross(minor);
	Eigen::Matrix3f rot; rot << -major, -minor, -cross;
	for (auto v : mesh.vertices()) points[v] = rot * (Eigen::Vector3f)(points[v]); // TODO Eigen calculations
	printf(" ([%f, %f, %f] / [%f, %f, %f] / [%f, %f, %f])\n", major(0), major(1), major(2), minor(0), minor(1), minor(2), cross(0), cross(1), cross(2));

	// Flip based on moment tests
	printf("Flipping based on moment test");
	Eigen::Vector3f flip = map.cwiseSign().cwiseProduct(map.cwiseProduct(map)).rowwise().sum().cwiseSign()
		.unaryExpr([](float e) -> float { return e == 0.f ? 1.f : e; }); // Change all 0s to 1s
	for (auto v : mesh.vertices()) points[v] = ((Eigen::Vector3f)points[v]).cwiseProduct(flip); // TODO Eigen calculations
	printf(" [%f, %f, %f]\n", flip(0), flip(1), flip(2));

	cout << map.cwiseSign() << endl;

	// Scale to unit volume
	printf("Scaling to unit volume");
	pmp::Point max = mesh.bounds().max();
	float scale = 1.f / (max[0] > max[1] ? max[0] : max[1] > max[2] ? max[1] : max[2]);
	map *= scale;
	printf(" ([%f, %f, %f] -> %f)\n", max[0], max[1], max[2], scale);

	cout << map << endl;

	// Write resampled mesh to disk
	// TODO file name/location
	printf("Writing mesh to disk\n");
	mesh.write(vars::GetAssetPath(path) + ".res.off");

	printf("Done\n");
}
