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

	vector<string> get_filenames(const string database)
    {
		vector<string> filenames = vector<string>();
        // Read filenames
        ifstream ifs(vars::GetAssetPath(database + "/files.json"));
        nlohmann::json files = nlohmann::json::parse(ifs);
        for (nlohmann::json::iterator it = files.begin(); it != files.end(); ++it) 
            for (nlohmann::json::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2) 
                filenames.push_back(*it2);
		return filenames;
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
	vector<string> filenames = database::get_filenames(database);
	// Process every file in the database
	for (string file : filenames)
	{
		size_t pos = file.find('.');
		string name = file.substr(0, pos);
		string ext = file.substr(pos + 1);
		 // Process all the unprocessed models
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
	vector<string> filenames = database::get_filenames(database);
	// Extract feature descriptors for every file in the database
	for (string file : filenames)
	{
		size_t pos = file.find('.');
		string name = file.substr(0, pos);
		string ext = file.substr(pos + 1);
		// Calculate feature for all the processed models
		extract(database, name + "_processed." + ext);
	}
}

void extract(const string database, const string in)
{
	// Get the mesh
	pmp::SurfaceMesh mesh = database::read_mesh(database, in);

	// Extract features
	auto globalDescriptors = descriptors::get_global_descriptors(mesh);
	auto shapeDescriptors  = descriptors::get_shape_descriptors(mesh, 10);

	// Write the descriptors to json
	// TODO maybe move to a seperate function
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
		{"diameter", globalDescriptors.diameter}/*,
		{"A3", shapeDescriptors.A3},
		{"D1", shapeDescriptors.D1},
		{"D2", shapeDescriptors.D2},
		{"D3", shapeDescriptors.D3},
		{"D4", shapeDescriptors.D4}*/
	};
	ofstream ofs(vars::GetAssetPath(database + "/featureDescriptors.json"));
	ofs << setw(4) << jsonDescriptors << endl; // TODO: removing setw(4) might improve filesize
	ofs.close();
}
