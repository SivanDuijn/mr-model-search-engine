#include "multimedia_processor.h"

int main(int argc, char *argv[])
{
	if (argc < 2) printf("Please supply an argument");
	// No switch for "strings" :(
	else if (!strcmp(argv[1], "debug"))       debug();
	else if (!strcmp(argv[1], "preprocess"))  
	{
		if (argc == 4) 
		{
			string s(argv[3]);
			size_t pos = s.find('.');
			string name = s.substr(0, pos);
			string ext = s.substr(pos + 1);
			preprocess(argv[2], s, name + "_processed." + ext);
		}
		else if (argc == 5)
			preprocess(argv[2], argv[3], argv[4]);
		else
			preprocess();
	}
	else if (!strcmp(argv[1], "preprocess-all")) preprocessAll();
	//else if (!strcmp(argv[1], "resample"))  resample();
	//else if (!strcmp(argv[1], "normalize")) normalize();
	else printf("Unknown argument");

	return 0;
}

void debug()
{
	printf("Echoing debug call\n");
}

void preprocessAll(const string database)
{
	vector<string> filenames;
	
	// Read in filenames
	ifstream ifs(vars::GetAssetPath(database + "/files.json"));
	nlohmann::json files = nlohmann::json::parse(ifs);
	for (nlohmann::json::iterator it = files.begin(); it != files.end(); ++it) 
		for (nlohmann::json::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2) 
  			filenames.push_back(*it2);

	for (string file : filenames)
	{
		size_t pos = file.find('.');
		string name = file.substr(0, pos);
		string ext = file.substr(pos + 1);
		preprocess(database, file, name + "_processed." + ext, false);
	}
}

void preprocess(const string database, const string in, const string out, const bool debug)
{
	// Get the mesh
	if (debug) printf("Loading mesh \"%s\" from %s\n", in.c_str(), database.c_str());
	pmp::SurfaceMesh mesh;
	mesh.read(vars::GetAssetPath(database + "/models/" + in));

	NormalizationStatistics beforeStats = CalculateNormalizationStats(mesh);

	// Preprocess the mesh
	resample(mesh, debug);
	normalize(mesh, debug);

	NormalizationStatistics afterStats = CalculateNormalizationStats(mesh);

	// Write resampled mesh to disk
	// TODO file name/location
	if (debug) printf("Writing mesh to disk\n");
	mesh.write(vars::GetAssetPath(database + "/models/" + out));

	// Write the normalizationStats to json
	ifstream ifs(vars::GetAssetPath(database + "/normalizationStats.json"));
	nlohmann::json normStats = nlohmann::json::parse(ifs);
	normStats[in] = { 
		{"nVertices", beforeStats.nVertices}, 
		{"nFaces", beforeStats.nFaces},
		{"aabbSize", beforeStats.boundingBoxSize},
		{"position", beforeStats.distBarycenterToOrigin},
		{"angleX", beforeStats.angleX},
		{"angleY", beforeStats.angleY},
		{"angleZ", beforeStats.angleZ},
		{"totalAngle", beforeStats.totalAngle},
		{"totalFlip", beforeStats.totalFlip}
	};
	normStats[out] = { 
		{"nVertices", afterStats.nVertices}, 
		{"nFaces", afterStats.nFaces},
		{"aabbSize", afterStats.boundingBoxSize},
		{"position", afterStats.distBarycenterToOrigin},
		{"angleX", afterStats.angleX},
		{"angleY", afterStats.angleY},
		{"angleZ", afterStats.angleZ},
		{"totalAngle", afterStats.totalAngle},
		{"totalFlip", afterStats.totalFlip}
	};
	ofstream ofs(vars::GetAssetPath(database + "/normalizationStats.json"));
	ofs << setw(4) << normStats << endl;

	printf("Preprocessed mesh \"%s\" from %s successfully, output: %s\n", in.c_str(), database.c_str(), out.c_str());
}

// Resample a mesh at the given path
void resample(pmp::SurfaceMesh &mesh, const bool debug)
{
	if (debug) printf("Mesh has %zu vertices\n", mesh.n_vertices());

	// Triangulate it if necessary
	if (!mesh.is_triangle_mesh())
	{
		if (debug) printf("Triangulating mesh\n");
		pmp::Triangulation tri = pmp::Triangulation(mesh);
		tri.triangulate();
	}

	// Example desired vertex count mentioned in technical tips
	const unsigned int des = 2500;

	// Subdivide and/or decimate depending on the vertex count
	while (mesh.n_vertices() < des)
	{
		if (debug) printf("Subdividing mesh");
		pmp::Subdivision div = pmp::Subdivision(mesh);
		div.loop();
		if (debug) printf(" to %zu vertices\n", mesh.n_vertices());
	}
	if (mesh.n_vertices() > des)
	{
		if (debug) printf("Decimating mesh");
		pmp::Decimation dec = pmp::Decimation(mesh);
		dec.decimate(des);
		if (debug) printf(" to %zu vertices\n", mesh.n_vertices());
	}
	else
	{
		if (debug) printf("Mesh contains exactly the right amount of vertices!\n");
	}

	if (debug) printf("Done\n");
}

// Normalize a mesh according to 4 steps:
// * Translate barycenter to origin
// * Align with coordinate frame
// * Flip based on moment test
// * Scale to unit volume
void normalize(pmp::SurfaceMesh &mesh, const bool debug)
{
	// Map the point list (aka array of Eigen::Matrix3f) to a single Eigen::MatrixXd
	pmp::VertexProperty points = mesh.get_vertex_property<pmp::Point>("v:point");
	Eigen::Map<Eigen::MatrixXf> map((float*)(points.data()), 3, mesh.n_vertices());

	auto f = mesh.faces();

	// Translate barycenter to origin
	if (debug) printf("Translating barycenter to origin");
	pmp::Point bcenter = pmp::centroid(mesh);
	map.colwise() -= (Eigen::Vector3f)bcenter;
	if (debug) printf(" (-[%f, %f, %f])\n", bcenter.data()[0], bcenter.data()[1], bcenter.data()[2]);

	// Align with coordinate frame
	if (debug) printf("Aligning with coordinate frame\n");
	// Compute the covariance matrix
	// https://stackoverflow.com/a/15142446
	// We don't have to center the sample matrix because we just centered the samples :)
	Eigen::Matrix3f cov = (map * map.adjoint()) / float(map.cols() - 1);
	// Get the eigenvectors
	Eigen::EigenSolver<Eigen::MatrixXf> solver;
	solver.compute(cov);
	Eigen::MatrixXf eigen = solver.eigenvectors().real();
	// Rotate the model
	for (auto v : mesh.vertices()) points[v] = eigen.inverse() * (Eigen::Vector3f)(points[v]); // TODO Eigen calculations

	// Flip based on moment tests
	if (debug) printf("Flipping based on moment test");
	Eigen::Vector3f flip = map.cwiseSign().cwiseProduct(map.cwiseProduct(map)).rowwise().sum().cwiseSign()
		.unaryExpr([](float e) -> float { return e == 0.f ? 1.f : e; }); // Change all 0s to 1s
	for (auto v : mesh.vertices()) points[v] = ((Eigen::Vector3f)points[v]).cwiseProduct(flip); // TODO Eigen calculations
	if (debug) printf(" [%f, %f, %f]\n", flip(0), flip(1), flip(2));

	// Scale to unit volume
	if (debug) printf("Scaling to unit volume");
	Eigen::MatrixXf bounds = Eigen::MatrixXf(3, 2);
	bounds << (Eigen::Vector3f)(mesh.bounds().min()), (Eigen::Vector3f)(mesh.bounds().max());
	float max = bounds.cwiseAbs().maxCoeff();
	float scale = 0.5f / max;
	map *= scale;
	if (debug) printf(" (%f => *%f)\n", max, scale);

	if (debug) printf("Done\n");
}
