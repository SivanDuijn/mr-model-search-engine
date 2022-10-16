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
	if (argc < 2) printf("Please supply an argument");

	else if (!strcmp(argv[1], "debug")) debug();

	else if (!strcmp(argv[1], "preprocess-all")) preprocessAll();

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

	else if (!strcmp(argv[1], "extract")) extract();

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

void extract(const string database, const string in)
{
	// Get the mesh
	pmp::SurfaceMesh mesh = database::read_mesh(database, in);

	// Check whether it is preprocessed
	assert(preprocessor::is_resampled(mesh));

	cout << "A3:\n" << descriptors::A3(mesh, 10) << endl;
	cout << "D1:\n" << descriptors::D1(mesh, 10) << endl;
	cout << "D2:\n" << descriptors::D2(mesh, 10) << endl;
	cout << "D3:\n" << descriptors::D3(mesh, 10) << endl;
}
