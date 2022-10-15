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
	printf_debug("Loading mesh \"%s\" from %s\n", in.c_str(), database.c_str());
	pmp::SurfaceMesh mesh;
	mesh.read(vars::GetAssetPath(database + "/models/" + in));

	cout << '\n' << descriptors::D1(mesh, 10) << endl;

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
