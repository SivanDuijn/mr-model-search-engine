#include "headers.h"

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
	vector<string> filenames;
	for (string file : database::get_filenames(database))
	{
		size_t pos = file.find('.');
		string name = file.substr(0, pos);
		string ext = file.substr(pos + 1);
		// Calculate descriptors for all the processed models
		filenames.push_back(name + "_processed." + ext);
	}

	// Extract features
	vector<descriptors::GlobalDescriptors> gds;
	vector<descriptors::ShapeDescriptors> sds;
	printf_debug("Getting global descriptors\n");
	descriptors::get_global_descriptors(database, filenames, gds);
	printf_debug("Getting shape descriptors\n");
	descriptors::get_shape_descriptors(database, filenames, sds, 10);

	ifstream ifs(vars::GetAssetPath(database + "/featureDescriptors.json"));
	nlohmann::json jsonDescriptors;
	if (!ifs.fail())
		jsonDescriptors = nlohmann::json::parse(ifs);
	for (size_t i = 0, nFiles = filenames.size(); i < nFiles; i++)
		jsonDescriptors[filenames[i]] = {
			{"area", gds[i].surfaceArea}, 
			{"AABBVolume", gds[i].AABBVolume},
			{"volume", gds[i].volume},
			{"compactness", gds[i].compactness},
			{"eccentricity", gds[i].eccentricity},
			{"diameter", gds[i].diameter},
			{"sphericity", gds[i].sphericity},
			{"rectangularity", gds[i].rectangularity},
			{"A3", sds[i].A3.bins.array()},
			{"D1", sds[i].D1.bins.array()},
			{"D2", sds[i].D2.bins.array()},
			{"D3", sds[i].D3.bins.array()},
			{"D4", sds[i].D4.bins.array()}
		};
	ofstream ofs(vars::GetAssetPath(database + "/featureDescriptors.json"));
	ofs << jsonDescriptors << endl; // TODO: removing setw(4) might improve filesize
	ofs.close();
}

void extract(const string database, const string in)
{
	// Get the mesh
	pmp::SurfaceMesh mesh = database::read_mesh(database, in);

	// Extract features
	printf_debug("Getting global descriptors\n");
	descriptors::get_global_descriptors(mesh);
	printf_debug("Getting shape descriptors\n");
	descriptors::get_shape_descriptors(mesh, 10);
}
