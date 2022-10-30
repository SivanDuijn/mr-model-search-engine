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
		if (out != "")
			preprocess(database, in, out);
		else if (ext != "")
			preprocess(database, in, inWithoutExt + "_processed." + ext);
		else 
			preprocess();

	else if (!strcmp(argv[1], "extract-all")) extractAll();

	else if (!strcmp(argv[1], "extract"))
		if (in != "")
			extract(database, in);
		else 
			extract();

	else if (!strcmp(argv[1], "compute-fvs") || !strcmp(argv[1], "compute-feature-vectors"))
		computeFeatureVectors();

	else if (!strcmp(argv[1], "query-model"))
		if (in != "")
			queryDatabaseModel(database, in);
		else
			queryDatabaseModel();

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

	// Write the normalization stats to json
	modelstats::WriteNormalizationStatistics(database, in, out, beforeStats, afterStats);

	printf("Preprocessed mesh \"%s\" from %s successfully, output: %s\n", in.c_str(), database.c_str(), out.c_str());
}

void extractAll(const string database)
{
	vector<string> filenames = database::get_filenames(database, true);

	// Extract features
	vector<descriptors::GlobalDescriptors> gds;
	vector<descriptors::ShapeDescriptors> sds;
	printf_debug("Getting global descriptors\n");
	descriptors::get_global_descriptors(database, filenames, gds);
	printf_debug("Getting shape descriptors\n");
	descriptors::get_shape_descriptors(database, filenames, sds, 10);

	ifstream ifs(vars::GetAssetPath(database + "/feature_descriptors.json"));
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
	ofstream ofs(vars::GetAssetPath(database + "/feature_descriptors.json"));
	ofs << setw(4) << jsonDescriptors << endl; // TODO: removing setw(4) might improve filesize
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

void computeFeatureVectors(const string database)
{
	vector<string> filenames = database::get_filenames(database, true);
	int n_models = filenames.size();
	ifstream ifs(vars::GetAssetPath(database + "/feature_descriptors.json"));
	nlohmann::json json_descriptors = nlohmann::json::parse(ifs);

	// Create vectors for shape descriptors
	vector<string> shape_descriptor_names{ "A3", "D1", "D2", "D3", "D4" };
	// Use the first model in the json to determine the binsize for each shape descriptor
	vector<size_t> shape_descriptor_bincount;
	auto firstModel = json_descriptors.begin().value();
	for (string sd : shape_descriptor_names)
		shape_descriptor_bincount.push_back(firstModel[sd].size());
	// Create a matrix for each shape descriptor where the rows represent the models
	vector<Eigen::MatrixXf> shape_fvs;
	for (size_t i = 0; i < shape_descriptor_names.size(); i++)
		shape_fvs.push_back(Eigen::MatrixXf(n_models, shape_descriptor_bincount[i]));

	// Create global descriptors matrix
	Eigen::MatrixXf global_fvs(n_models, 8);

	size_t i = 0;
	for (int i = 0; i < n_models; i++)
	{
		auto descriptors = json_descriptors[filenames[i]];
		global_fvs.row(i) = Eigen::Matrix<float, 8, 1>(
			descriptors["area"].get<float>(),
			descriptors["AABBVolume"].get<float>(),
			descriptors["volume"].get<float>(),
			descriptors["compactness"].get<float>(),
			descriptors["eccentricity"].get<float>(),
			descriptors["diameter"].get<float>(),
			descriptors["sphericity"].get<float>(),
			descriptors["rectangularity"].get<float>()
		);

		for (size_t sd_i = 0; sd_i < shape_descriptor_names.size(); sd_i++) 
		{
			auto descriptor = descriptors[shape_descriptor_names[sd_i]];
			size_t sdj = 0;
			for (auto it2 = descriptor.begin(); it2 != descriptor.end(); ++it2, sdj++)
				shape_fvs[sd_i](i,sdj) = it2.value().get<float>();
		}
	}

	// Standardize global descriptors using the standard deviation
	Eigen::Matrix<float, 1, 8> global_mean = global_fvs.colwise().mean();
	Eigen::Matrix<float, 1, 8> global_sd = ((global_fvs.rowwise() - global_mean).array().square().colwise().sum() / (n_models - 1)).sqrt();
	auto standardized_global_fvs = (global_fvs.rowwise() - global_mean).array().rowwise() / global_sd.array();

	cout << global_mean << endl;
	cout << global_sd << endl;

	// Calculate mean and SD of shape descriptor distance
	Eigen::MatrixXf shape_dists(shape_descriptor_names.size(), (n_models*(n_models - 1)) / 2); // shape descriptor distances, row for each shape descriptor
	for (size_t sd_i = 0, n_fvs = shape_fvs.size(); sd_i < n_fvs; sd_i++) 
		for (int i = 0, d_i = 0; i < n_models; i++) 
			for (int j = i + 1; j < n_models; j++, d_i++)
			{
				// sd_i is the shape descriptor index, A3 D1 D2 ...
				// i j loop through all the models but i != j and j > i, so we don't have duplicate distances
				// d_i is the distance index, just a counter for the distances

				// Earth Movers Distance
				shape_dists(sd_i, d_i) = utils::EarthMoversDistance(shape_fvs[sd_i].row(i), shape_fvs[sd_i].row(j));
			}
	
	auto shape_dists_mean = shape_dists.rowwise().mean();
	auto shape_dists_sd = ((shape_dists.colwise() - shape_dists_mean).array().square().rowwise().sum() / (shape_dists.cols() - 1)).sqrt();
	// auto standardized_shape_dists = (shape_dists.colwise() - shape_dists_mean).array().colwise() / shape_dists_sd.array();
	auto standardized_shape_dists = shape_dists.array().colwise() / shape_dists_sd.array();
	cout << shape_dists_mean.transpose() << endl;
	cout << shape_dists_sd.transpose() << endl;

	// Write feature vectors and mean and sd to json
	// separate global descriptors and shape descriptors
	nlohmann::json json_fvs;
	nlohmann::json json_models;
	for (int i = 0; i < n_models; i++)
	{
		vector<Eigen::ArrayXf> s;
		for (auto a : shape_fvs)
			s.push_back(a.row(i).array());
		json_models[filenames[i]] = {
			{"global", standardized_global_fvs.row(i).array()},
			{"shape", s}
		};
	}
	json_fvs["global_mean"] = global_mean.array();
	json_fvs["global_sd"] = global_sd.array();
	json_fvs["shape_dists_mean"] = shape_dists_mean.array();
	json_fvs["shape_dists_sd"] = shape_dists_sd.array();
	json_fvs["models"] = json_models;
	ofstream ofs(vars::GetAssetPath(database + "/feature_vectors.json"));
	ofs << setw(4) << json_fvs << endl; // TODO: removing setw(4) might improve filesize
	ofs.close();

	// Calculate distance matrix
	vector<float> dists((n_models*(n_models - 1)) / 2);
	for (int i = 0, d_i = 0; i < n_models; i++) 
	{
		printf("%i of %i\n", i, n_models);
		for (int j = i + 1; j < n_models; j++, d_i++)
		{
			float global_dist_2 = (standardized_global_fvs.row(i) - standardized_global_fvs.row(j)).array().square().sum();
			float shape_dist_2 = standardized_shape_dists.col(d_i).array().square().sum();
			dists[d_i] = sqrtf(global_dist_2 + shape_dist_2);
		}
	}
	
	nlohmann::json json_dists = dists;
	ofs = ofstream(vars::GetAssetPath(database + "/dist_matrix.json"));
	ofs << json_dists << endl;
	ofs.close();
}

void queryDatabaseModel(const string database, const string in)
{
	vector<string> filenames = database::get_filenames(database, true);
	int n_models = filenames.size();

	// Read in distance matrix
	ifstream ifs(vars::GetAssetPath(database + "/dist_matrix.json"));
	nlohmann::json json_dist_matrix;
	if (ifs.fail())
		return;
	json_dist_matrix = nlohmann::json::parse(ifs);
	ifs.close();

	vector<float> dists((n_models * (n_models-1)) / 2);
	int i = 0;
	for (auto it = json_dist_matrix.begin(); it != json_dist_matrix.end(); ++it, i++) 
  		dists[i] = *it;

	// Give top n models for model x
	// get index of model
	int m_i = 0;
	for (; m_i < n_models; m_i++)
		if (filenames[m_i] == in)
			break;

	vector<float> q_dists(n_models); 

	int d_i = m_i - 1;
	i = 0;
	for (int j = n_models - 2; i < m_i; d_i += j, j--, i++) 
		q_dists[i] = dists[d_i]; 

	d_i++;
	i++;	
	for (; i < n_models; i++, d_i++)
		q_dists[i] = dists[d_i];

	auto indices = n_smallest_indices(q_dists.begin(), q_dists.end(), 10);

	for (auto index : indices)
		cout << filenames[index] << " " << q_dists[index] << endl;

	
	// // manually calculate distance between two models to test
	// string ma = "24_processed.off";
	// string mb = "31_processed.off";

	// ifs = ifstream(vars::GetAssetPath(database + "/feature_vectors.json"));
	// nlohmann::json json_feature_vectors = nlohmann::json::parse(ifs);

	// Eigen::VectorXf dists_mean = utils::JSONArrayToVector(json_feature_vectors["shape_dists_mean"]);
	// Eigen::VectorXf dists_sd = utils::JSONArrayToVector(json_feature_vectors["shape_dists_sd"]);

	// auto ma_json = json_feature_vectors["models"][ma];
	// auto mb_json = json_feature_vectors["models"][mb];
	// Eigen::VectorXf ma_global = utils::JSONArrayToVector(ma_json["global"]);
	// Eigen::VectorXf mb_global = utils::JSONArrayToVector(mb_json["global"]);

	// vector<string> shape_descriptor_names{ "A3", "D1", "D2", "D3", "D4" };
	// vector<Eigen::VectorXf> ma_shape_hists;
	// vector<Eigen::VectorXf> mb_shape_hists;
	// auto mb_json_shape = mb_json["shape"];
	// auto ma_json_shape = ma_json["shape"];
	// for (int j = 0; j < shape_descriptor_names.size(); j++)
	// {
	// 	ma_shape_hists.push_back(utils::JSONArrayToVector(ma_json_shape[j]));
	// 	mb_shape_hists.push_back(utils::JSONArrayToVector(mb_json_shape[j]));
	// }

	// float global = (ma_global - mb_global).array().square().sum();
	// cout << global << endl;

	// float shape = 0;
	// for (int j = 0; j < shape_descriptor_names.size(); j++)
	// {
	// 	float emd = utils::EarthMoversDistance(ma_shape_hists[j], mb_shape_hists[j]);
	// 	// emd = (emd - dists_mean(j)) / dists_sd(j);
	// 	emd /= dists_sd(j);
	// 	shape += emd*emd;
	// }

	// cout << shape << endl;
	// cout << sqrtf(global + shape) << endl;
}
