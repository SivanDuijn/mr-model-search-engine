#include "headers.h"

int main(int argc, char *argv[])
{
	// try to get databasename, in and out filenames
	string database = ""; string in = ""; string inWithoutExt = ""; string ext = ""; string out = "";
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

	else if (!strcmp(argv[1], "extract"))
	{
		if (out != "")
			extract(database, in, out);
		else if (inWithoutExt != "")
			extract(database, in, inWithoutExt + "_features.json");
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
	printf_debug("Loading mesh \"%s\" from %s\n", in.c_str(), database.c_str());
	pmp::SurfaceMesh mesh;
	mesh.read(vars::GetAssetPath(database + "/models/" + in));

	modelstats::NormalizationStatistics beforeStats;
	modelstats::NormalizationStatistics afterStats;

	// Preprocess the mesh
	preprocessor::resample(mesh, beforeStats, afterStats);
	preprocessor::normalize(mesh, beforeStats, afterStats);

	// Write resampled mesh to disk
	printf_debug("Writing mesh to disk\n");
	mesh.write(vars::GetAssetPath(database + "/models/" + out));

	// Write the normalizationStats to json
	modelstats::WriteNormalizationStatistics(database, in, out, beforeStats, afterStats);

	printf("Preprocessed mesh \"%s\" from %s successfully, output: %s\n", in.c_str(), database.c_str(), out.c_str());
}

void extract(const string database, const string in, const string out)
{
	// Get the mesh
	printf_debug("Loading mesh \"%s\" from %s\n", in.c_str(), database.c_str());
	pmp::SurfaceMesh mesh;
	mesh.read(vars::GetAssetPath(database + "/models/" + in));

	cout << '\n' << descriptors::D1(mesh, 10) << endl;

	global_descriptors::CalcAll(mesh);
}
