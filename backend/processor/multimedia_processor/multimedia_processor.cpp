#include "headers.h"

// TODO move to seperate file
// I hate header files :(
namespace database 
{
    // Read a mesh from a database
    pmp::SurfaceMesh read_mesh(const string database, const string file)
    {
	    printf_debug("Loading mesh \"%s\" from %s\n", file.c_str(), database.c_str());
	    pmp::SurfaceMesh mesh;
	    mesh.read(vars::GetAssetPath(database + "/models/" + file));
        return mesh;
    }

    // Write a mesh to a database
    void write_mesh(pmp::SurfaceMesh &mesh, const string database, const string file)
    {
	    printf_debug("Writing mesh \"%s\" to disk\n", file.c_str());
	    mesh.write(vars::GetAssetPath(database + "/models/" + file));
    }
}

int main(int argc, char *argv[])
{
	// try to get databasename, in and out filenames
	string database = "", in = "", inWithoutExt = "", ext = "", out = "";
	if (argc == 4) 
	{
		database = argv[2];
		in = argv[3];
		string s(argv[3]);
		size_t pos = s.find('.');
		inWithoutExt = s.substr(0, pos);
		ext = s.substr(pos + 1);
	}
	else if (argc == 5)
	{
		database = argv[2];
		in = argv[3];
		out = argv[4];
	}

	if (argc < 2) printf("Please supply an argument");

	else if (!strcmp(argv[1], "debug")) debug();

	else if (!strcmp(argv[1], "preprocess-all")) preprocessAll();

	else if (!strcmp(argv[1], "preprocess"))  
	{
		if (out != "")
			preprocess(database, in, out);
		else if (ext != "")
			preprocess(database, in, inWithoutExt + "_processed." + ext);
		else 
			preprocess();
	}

	else if (!strcmp(argv[1], "extract-all")) extractAll();

	else if (!strcmp(argv[1], "extract"))
	{
		if (in != "")
			extract(database, in);
		else 
			extract();
	}

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
	utils::GetAllFilenamesInDatabase(database, filenames);
	// Process every file in the database
	for (string file : filenames)
	{
		size_t pos = file.find('.');
		string name = file.substr(0, pos);
		string ext = file.substr(pos + 1);
		preprocess(database, file, name + "_processed." + ext);
	}
}

void preprocess(const string database, const string in, const string out)
{
	// Get the mesh
	pmp::SurfaceMesh mesh = database::read_mesh(database, in);

	// Preprocess the mesh
	modelstats::NormalizationStatistics beforeStats;
	modelstats::NormalizationStatistics afterStats;
	preprocessor::resample(mesh, beforeStats, afterStats);
	preprocessor::normalize(mesh, beforeStats, afterStats);

	// Write resampled mesh to disk
	database::write_mesh(mesh, database, out);

	// Write the normalizationStats to json
	modelstats::WriteNormalizationStatistics(database, in, out, beforeStats, afterStats);

	printf("Preprocessed mesh \"%s\" from %s successfully, output: %s\n", in.c_str(), database.c_str(), out.c_str());
}

void extractAll(const string database)
{
	vector<string> filenames;
	utils::GetAllFilenamesInDatabase(database, filenames);

	// Extract feature descriptors for every file in the database
	for (string file : filenames)
	{
		size_t pos = file.find('.');
		string name = file.substr(0, pos);
		string ext = file.substr(pos + 1);
		extract(database, name + "_processed." + ext); // Calculate feature for all the processed models
	}
}

void extract(const string database, const string in)
{
	// Get the mesh
	pmp::SurfaceMesh mesh = database::read_mesh(database, in);

	auto globalDescriptors = global_descriptors::CalcAll(mesh);

	// Write the descriptors to json
	ifstream ifs(vars::GetAssetPath(database + "/featureDescriptors.json"));
	nlohmann::json jsonDescriptors;
	if (!ifs.fail())
		jsonDescriptors = nlohmann::json::parse(ifs);
	jsonDescriptors[in] = {
		{"area", globalDescriptors.surfaceArea}, 
		{"AABBVolume", globalDescriptors.AABBVolume},
		{"volume", globalDescriptors.volume},
		{"compactness", globalDescriptors.compactness},
		{"eccentricity", globalDescriptors.eccentricity},
		{"diameter", globalDescriptors.diameter}
	};
	ofstream ofs(vars::GetAssetPath(database + "/featureDescriptors.json"));
	ofs << setw(4) << jsonDescriptors << endl; // TODO: removing setw(4) might improve filesize
	ofs.close();
}

// TODO remove
void extractOld(const string database, const string in)
{
	// Get the mesh
	pmp::SurfaceMesh mesh = database::read_mesh(database, in);

	// Check whether it is preprocessed
	assert(preprocessor::is_resampled(mesh));

	cout << "A3:\n" << descriptors::A3(mesh, 10) << endl;
	cout << "D1:\n" << descriptors::D1(mesh, 10) << endl;
	cout << "D2:\n" << descriptors::D2(mesh, 10) << endl;
	cout << "D3:\n" << descriptors::D3(mesh, 10) << endl;
	cout << "D4:\n" << descriptors::D4(mesh, 10) << endl;
}
