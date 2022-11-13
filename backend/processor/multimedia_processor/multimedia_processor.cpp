#include "headers.h"

int main(int argc, char *argv[])
{
	// try to get databasename, in and out filenames
	string database = "", in = "", inWithoutExt = "", ext = "", out = "";
	if (argc == 3)
		database = argv[2];
	else if (argc == 4) 
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

	if (database != "")
		Database::SetDatabaseDir(database);
	else
		Database::SetDatabaseDir("../../frontend/model-viewer/public/PSBDatabase");


	if (argc < 2) printf("Please supply an argument");

	else if (!strcmp(argv[1], "debug")) debug();

	else if (!strcmp(argv[1], "preprocess-all")) 
		preprocess_all();

	else if (!strcmp(argv[1], "preprocess"))  
		if (out != "")
			preprocess(in, out);
		else if (ext != "")
			preprocess(in, inWithoutExt + "_processed." + ext);
		else 
			preprocess();

	else if (!strcmp(argv[1], "extract-all"))
		extract_all();

	else if (!strcmp(argv[1], "extract"))
		extract();

	else if (!strcmp(argv[1], "compute-fvs") || !strcmp(argv[1], "compute-feature-vectors"))
		compute_feature_vectors();

	else if (!strcmp(argv[1], "compute-all-closest"))
		compute_closest_models();

	else if (!strcmp(argv[1], "evaluate"))
		evaluate();

	else if (!strcmp(argv[1], "query-database-model"))
		query_database_model();

	else if (!strcmp(argv[1], "query-model"))
		if (in != "")
			query_top_k_models(in, 10);
		else 
			cout << "{\"error\": \"Wrong arguments!\"}";

	else printf("Unknown argument");

	return 0;
}

void debug()
{
	printf("Echoing debug call\n");
}

void preprocess_all()
{
	vector<string> filenames = Database::GetFilenames();
	// Process every file in the database
	for (string file : filenames)
		 // Process all the unprocessed models
		preprocess(file, utils::to_processed(file));
}

void preprocess(const string in, const string out)
{
	// Get the mesh
	pmp::SurfaceMesh mesh = Database::ReadMeshFromDatabase(in);

	// Preprocess the mesh
	Database::NormalizationStatistics beforeStats;
	Database::NormalizationStatistics afterStats;
	preprocessor::resample(mesh, beforeStats, afterStats);
	preprocessor::normalize(mesh, beforeStats, afterStats);

	// Write resampled mesh to disk
	Database::WriteMesh(mesh, out);

	// Write the normalization stats to json
	Database::WriteStats(in, out, beforeStats, afterStats);

	printf("Preprocessed mesh \"%s\" from %s successfully, output: %s\n", in.c_str(), Database::GetDatabaseDir().c_str(), out.c_str());
}

void extract_all()
{
	vector<string> filenames = Database::GetFilenames(true);

	// Extract features
	vector<descriptors::GlobalDescriptors> gds;
	vector<descriptors::ShapeDescriptors> sds;
	printf_debug("Getting global descriptors\n");
	descriptors::get_global_descriptors(filenames, gds);
	printf_debug("Getting shape descriptors\n");
	descriptors::get_shape_descriptors(filenames, sds);

	Database::WriteDescriptors(filenames, gds, sds);
}

void extract(const string in)
{
	// Get the mesh
	pmp::SurfaceMesh mesh = Database::ReadMeshFromDatabase(in);

	// Extract features
	printf_debug("Getting global descriptors\n");
	descriptors::get_global_descriptors(mesh);
	printf_debug("Getting shape descriptors\n");
	descriptors::get_shape_descriptors(mesh);

	// TODO write?
}

void compute_feature_vectors()
{
	vector<string> filenames = Database::GetFilenames(true);
	int n_models = filenames.size();

	Eigen::MatrixXf global_fvs = Database::GetGlobalFVS();
	vector<Eigen::MatrixXf> shape_fvs = Database::GetShapeFVS();

	// Standardize global descriptors using the standard deviation
	Eigen::Matrix<float, 1, 8> global_mean = global_fvs.colwise().mean();
	Eigen::Matrix<float, 1, 8> global_sd = ((global_fvs.rowwise() - global_mean).array().square().colwise().sum() / (n_models - 1)).sqrt();
	auto standardized_global_fvs = (global_fvs.rowwise() - global_mean).array().rowwise() / global_sd.array();

	cout << global_mean << endl;
	cout << global_sd << endl;

	// Calculate mean and SD of shape descriptor distance
	// shape descriptor distances, row for each shape descriptor
	Eigen::MatrixXf shape_dists(shape_fvs.size(), (n_models*(n_models - 1)) / 2);
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

	// Write feature vectors to database as json
	Database::WriteFVS(global_mean, global_sd, shape_dists_mean, shape_dists_sd, shape_fvs, standardized_global_fvs);

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
	
	// Write distance matrix to database as json
	Database::WriteDistMatrix(dists);
}

vector<tuple<int,float>> query_database_model(const string in, size_t k)
{
	vector<string> filenames = Database::GetFilenames(true);
	int n_models = filenames.size();

	vector<float> dists = Database::GetDistMatrix();

	// Give top n models for model x
	// get index of model
	int m_i = 0;
	for (; m_i < n_models; m_i++)
		if (filenames[m_i] == in)
			break;

	vector<float> q_dists(n_models); 

	int d_i = m_i - 1;
	int i = 0;
	for (int j = n_models - 2; i < m_i; d_i += j, j--, i++) {
		q_dists[i] = dists[d_i]; 
	}

	d_i++;
	i++;	
	for (; i < n_models; i++, d_i++)
		q_dists[i] = dists[d_i];

	auto indices = n_smallest_indices(q_dists.begin(), q_dists.end(), k);

	vector<tuple<int,float>> closest;
	for (auto index : indices)
		closest.push_back(std::make_pair(index,q_dists[index]));

	return closest;
}

void compute_closest_models()
{
	auto filenames = Database::GetFilenames();

	nlohmann::json json_closest;
	for (string file : filenames)
	{
		size_t pos = file.find('.');
		string name = file.substr(0, pos);
		string ext = file.substr(pos + 1);

		auto closest = query_database_model(name + "_processed." + ext, 11);
	
		closest.erase(closest.begin());
		vector<tuple<string,float>> cv;
		for (auto c : closest)
			cv.push_back(make_pair(filenames[get<0>(c)], get<1>(c)));
		json_closest[file] = cv;
	}

	ofstream ofs = ofstream(Database::GetDatabaseDir() + "/closest_models.json");
	ofs << setw(4) << json_closest << endl;
	ofs.close();
}

// Calculate the top k closest models 
void query_top_k_models(const string file, const int k)
{
	pmp::SurfaceMesh mesh = Database::ReadMesh(file);

	// Preprocess the mesh
	Database::NormalizationStatistics beforeStats;
	Database::NormalizationStatistics afterStats; 
	preprocessor::resample(mesh, beforeStats, afterStats);
	preprocessor::normalize(mesh, beforeStats, afterStats);

	// Extract features
	descriptors::GlobalDescriptors gd = descriptors::get_global_descriptors(mesh);
	descriptors::ShapeDescriptors sd = descriptors::get_shape_descriptors(mesh, 10);

	// Create a matrix for each shape descriptor where the rows represent the models
	vector<Eigen::Vector<float, 10>> q_shape_fv;
	// Create global descriptors matrix
	Eigen::Vector<float, 8> q_global_fv(
		gd.surfaceArea,
		gd.AABBVolume,
		gd.volume,
		gd.compactness,
		gd.eccentricity,
		gd.diameter,
		gd.sphericity,
		gd.rectangularity
	);

	q_shape_fv.push_back(sd.A3.bins);
	q_shape_fv.push_back(sd.D1.bins);
	q_shape_fv.push_back(sd.D2.bins);
	q_shape_fv.push_back(sd.D3.bins);
	q_shape_fv.push_back(sd.D4.bins);

	// Get global mean and global SD
	ifstream ifs = ifstream(Database::GetDatabaseDir() + "/feature_vectors.json");
	if (ifs.fail()) {
		cout << "{\"error\":\"Something went wrong when loading the feature vectors!\"}" << endl;
		return;
	}
	nlohmann::json json_feature_vectors = nlohmann::json::parse(ifs);
	Eigen::VectorXf global_mean = utils::json_array_to_vector(json_feature_vectors["global_mean"]);
	Eigen::VectorXf global_sd = utils::json_array_to_vector(json_feature_vectors["global_sd"]);
	q_global_fv = (q_global_fv - global_mean).array() / global_sd.array();

	Eigen::VectorXf dists_sd = utils::json_array_to_vector(json_feature_vectors["shape_dists_sd"]);

	// Go through all models and calculate all distances
	auto processed_filenames = Database::GetFilenames(true);
	vector<float> dists;
	for (string file : processed_filenames)
	{
		Eigen::VectorXf m_global_fv = utils::json_array_to_vector(json_feature_vectors["models"][file]["global"]);

		// Go through all shape descriptors
		size_t i = 0;
		float shape_dist_2 = 0;
		for (auto it = json_feature_vectors["models"][file]["shape"].begin(); it != json_feature_vectors["models"][file]["shape"].end(); ++it, i++)
		{
			Eigen::VectorXf m_shape_fv = utils::json_array_to_vector(it.value());
			float emd = utils::EarthMoversDistance(q_shape_fv[i], m_shape_fv);
			emd /= dists_sd(i);
			shape_dist_2 += emd*emd;
		}

		float global_dist_2 = (q_global_fv - m_global_fv).array().square().sum();
		dists.push_back(sqrtf(global_dist_2 + shape_dist_2));
	}

	auto indices = n_smallest_indices(dists.begin(), dists.end(), k);

	// Create output json
	nlohmann::json output_json;
	nlohmann::json top_k;
	auto filenames = Database::GetFilenames(false);
	for (size_t i = 0; i < indices.size(); i++)
		top_k.push_back({ {"name", filenames[indices[i]]}, {"dist", dists[indices[i]]} });
	output_json["top_k"] = top_k;
	output_json["stats"] = Database::StatsToJSON(afterStats);
	output_json["descriptors"] = Database::DescriptorsToJSON(gd, sd);
	output_json["processed_model"] = utils::mesh_to_off_string(mesh);

	cout << setw(4) << output_json << endl;
}

void evaluate(const size_t k)
{
	printf_debug("Evaluating performance on database %s\n", Database::GetDatabaseDir().c_str());
	map<string, int>              count_class;
	int                           count_total = 0;
	map<string, float>            quality_class;
	float			              quality_total = 0;
	map<string, map<string, int>> confusion;

	// Get the closest models
	ifstream ifs(Database::GetDatabaseDir() + "/closest_models.json");
	nlohmann::json closest_json = nlohmann::json::parse(ifs);
	ifs.close();

	// Go over all files in the database
	auto filenames = Database::GetFilenames();
	for (string file : filenames)
	{
		printf_debug("Evaluating for file %s", file.c_str());
		string ground_truth = Database::GetClass(file);
		size_t positive_true = 0,
			   positive_false = 0;
		count_class[ground_truth]++;
		count_total++;
		printf_debug(" (class %s)\n", ground_truth.c_str());

		// Go over all the top k models
		printf_debug("Returned ");
		nlohmann::json top_10_closest_json = closest_json[file];
		for (auto it = top_10_closest_json.begin(), end = top_10_closest_json.end(); it < end; ++it) 
		{
			string result = Database::GetClass(it.value()[0]);
			printf_debug("%s, ", result.c_str());

			// Obtain result stats
			if (result == ground_truth) positive_true++;
			else positive_false++;
			confusion[ground_truth][result]++;
		}

		// Apply the quality metric
		// Ratio of negative/k for now
		float quality = (float)positive_false / k;
		quality_class[ground_truth] += quality;
		quality_total               += quality;
		printf_debug("quality: %f\n", quality);
	}

	// Normalize the quality
	for (auto iter = quality_class.begin(), end = quality_class.end(); iter != end; iter++)
		iter->second /= (float)count_class[iter->first];
	quality_total /= count_total;

	// Print some information
	printf_debug("Class performance: ");
	for (auto iter = quality_class.begin(), end = quality_class.end(); iter != end; iter++)
		printf_debug("%s=%f, ", iter->first.c_str(), iter->second);
	cout << "total performance: " << to_string(quality_total) << endl;
	printf_debug("Confusion:\n");
	for (auto iter1 = confusion.begin(), end1 = confusion.end(); iter1 != end1; iter1++)
	{
		printf_debug("%s: ", iter1->first.c_str());
		for (auto iter2 = iter1->second.begin(), end2 = iter1->second.end(); iter2 != end2; iter2++)
			printf_debug("%s=%i, ", iter2->first.c_str(), iter2->second);
		printf_debug("\n");
	}
	Database::WriteConfusionMatrix(confusion);
}
