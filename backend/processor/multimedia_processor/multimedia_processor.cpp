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