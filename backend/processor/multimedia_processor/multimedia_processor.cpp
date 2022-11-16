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

	else if (!strcmp(argv[1], "compute-ANN"))
		compute_ann();

	else if (!strcmp(argv[1], "evaluate"))
		evaluate();

	else if (!strcmp(argv[1], "query-database-model"))
		query_database_model();

	else if (!strcmp(argv[1], "query-database-model-ann"))
		query_database_model_ann();

	else if (!strcmp(argv[1], "query-model"))
		if (in != "")
			query_top_k_models(in, 10);
		else 
			cout << "{\"error\": \"Wrong arguments!\"}";

	else if (!strcmp(argv[1], "compute-all-closest"))
		compute_closest_models();
		
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
	for (string file : filenames)
		preprocess(file, utils::to_processed(file));
}

void preprocess(const string in, const string out)
{
	// Get the mesh
	pmp::SurfaceMesh mesh = Database::ReadMeshFromDatabase(in);

	// Preprocess the mesh
	printf_debug("Preprocessing mesh for\"%s\" from %s", in.c_str(), Database::GetDatabaseDir().c_str());
	Database::NormalizationStatistics beforeStats;
	Database::NormalizationStatistics afterStats;
	preprocessor::resample(mesh, beforeStats, afterStats);
	preprocessor::normalize(mesh, beforeStats, afterStats);

	// Write resampled mesh to disk
	Database::WriteMesh(mesh, out);

	// Write the normalization stats to json
	Database::WriteStats(in, out, beforeStats, afterStats);
}

void extract_all()
{
	vector<string> filenames = Database::GetFilenames(true);

	// Extract features
	printf_debug("Extracting descriptors for all meshes in %s\n", Database::GetDatabaseDir().c_str());
	vector<descriptors::GlobalDescriptors> gds;
	vector<descriptors::ShapeDescriptors> sds;
	printf_debug("Getting global descriptors\n");
	descriptors::get_global_descriptors(filenames, gds);
	printf_debug("Getting shape descriptors\n");
	descriptors::get_shape_descriptors(filenames, sds);

	// Standardize global descriptors
	Eigen::MatrixXf standardized_global_fvs;
	Eigen::VectorXf global_mean;
	Eigen::VectorXf global_sd;
	descriptors::standardize(gds, standardized_global_fvs, global_mean, global_sd);

	// Standardize shape descriptor distances
	Eigen::MatrixXf standardized_shape_dists;
	Eigen::VectorXf shape_dists_mean;
	Eigen::VectorXf shape_dists_sd;
	descriptors::standardize_dists(sds, standardized_shape_dists, shape_dists_mean, shape_dists_sd);

	// Write feature vectors to database as json
	Database::WriteFVS(standardized_global_fvs, sds, global_mean, global_sd, shape_dists_mean, shape_dists_sd);

	// Calculate distance matrix
	// TODO: put in separate function, but requires recalculating shape distances
	size_t n_models = filenames.size();
	vector<float> dists((n_models*(n_models - 1)) / 2);
	for (int i = 0, d_i = 0; i < n_models; i++) 
		for (int j = i + 1; j < n_models; j++, d_i++)
		{
			float global_dist = distance::global_vf_distance(standardized_global_fvs.row(i), standardized_global_fvs.row(j));
			// float shape_dist = standardized_shape_dists.col(d_i).array().sum();
			float shape_dist = standardized_shape_dists.col(d_i).array().square().sum();
			dists[d_i] = distance::combine_global_shape_distance(global_dist, shape_dist, standardized_shape_dists.rows());
		}

	Database::WriteDistMatrix(dists);
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

void compute_ann()
{
	Eigen::MatrixXf global_fvs = Database::GetGlobalFVS();
	vector<Eigen::MatrixXf> shape_fvs = Database::GetShapeFVS();

	// ANN Index
	printf_debug("Compiling Annoy index\n");
	const size_t fvs_count = global_fvs.rows(),
				 global_fvs_size = global_fvs.cols(),
				 shape_fvs_size = shape_fvs.size(),
				 shape_fvs_bins = shape_fvs[0].cols(),
				 fvs_size = global_fvs_size + shape_fvs_size * shape_fvs_bins;
	//printf_debug("Vector count: %zu\nVector size: %zu+%zu*%zu=%zu\n", fvs_count, global_fvs_size, shape_fvs_size, shape_fvs_bins, fvs_size);

	// Create and populate the Annoy instance
	AnnoyIndex index(fvs_size);
	float* fv = (float*) malloc(fvs_size*sizeof(float));
	for (size_t i = 0; i < fvs_count; i++)
	{
		// Compile the feature vector
		memcpy(fv, global_fvs.row(i).data(), global_fvs_size*sizeof(float));
		for (int desc = 0, indx = global_fvs_size; desc < shape_fvs_size; desc++, indx += shape_fvs_bins)
			memcpy(fv + indx, shape_fvs[desc].row(i).data(), shape_fvs_bins*sizeof(float));
		index.add_item(i, fv);
	}

	// Build and save the tree
	printf_debug("Building ANN tree\n");
	index.build(2 * fvs_size);
	printf_debug("Saving tree\n");
	Database::WriteAnnoyIndex(index);
}

vector<tuple<int,float>> query_database_model(const string in, const size_t k)
{
	printf_debug("Querying for %zu closest models to %s\n", k, in.c_str());
	const vector<float> dists = Database::GetDistMatrix();
	const size_t m_i = Database::GetModelIndex(in);
	const size_t n_models = Database::GetDatabaseSize();
	vector<float> q_dists(n_models); 

	// Get the k closest neighbours
	int d_i = m_i - 1, i = 0;
	for (int j = n_models - 2; i < m_i; d_i += j, j--, i++)
		q_dists[i] = dists[d_i]; 
	d_i++, i++;	
	for (; i < n_models; i++, d_i++)
		q_dists[i] = dists[d_i];
	auto indices = n_smallest_indices(q_dists.begin(), q_dists.end(), k);

	// Pack the information
	vector<tuple<int, float>> ret;
	for (auto index : indices)
	{
		ret.push_back(std::make_pair(index, q_dists[index]));
		printf_debug("%zu: %f, ", index, q_dists[index]);
	}
	printf_debug("\n");
	return ret;
}

vector<tuple<int, float>> query_database_model_ann(const string in, const size_t k)
{
	printf_debug("Querying for %zu closest models to %s using ANN\n", k, in.c_str());
	const AnnoyIndex index = Database::GetAnnoyIndex();
	const size_t m_i = Database::GetModelIndex(in);

	// Get the k closest neighbours
	std::vector<size_t> closest;
	std::vector<float> dist;
	index.get_nns_by_item(m_i, k, index.get_n_items(), &closest, &dist);
	
	// Pack the information
	vector<tuple<int, float>> ret;
	for(int i = 0; i < k; i++)
	{
		ret.push_back(std::make_pair(closest[i], dist[i]));
		printf_debug("%zu: %f, ", closest[i], dist[i]);
	}
	printf_debug("\n");
	return ret;
}

void compute_closest_models()
{
	auto filenames = Database::GetFilenames();

	nlohmann::json json_closest;
	for (string file : filenames)
	{
		auto closest = query_database_model(utils::to_processed(file), 11);
	
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

	vector<Eigen::VectorXf> q_shape_fv;
	q_shape_fv.push_back(sd.A3.bins);
	q_shape_fv.push_back(sd.D1.bins);
	q_shape_fv.push_back(sd.D2.bins);
	q_shape_fv.push_back(sd.D3.bins);
	q_shape_fv.push_back(sd.D4.bins);


	Eigen::MatrixXf global_fvs = Database::GetGlobalFVS();
	vector<Eigen::MatrixXf> shape_fvs = Database::GetShapeFVS();

	Eigen::VectorXf global_mean = Database::GetGlobalMean();
	Eigen::VectorXf global_sd = Database::GetGlobalSD();
	q_global_fv = (q_global_fv - global_mean).array() / global_sd.array();

	Eigen::VectorXf shape_dists_sd = Database::GetShapeDistsSD();

	// Go through all models and calculate all distances
	vector<float> dists;
	for (size_t i = 0, n_models = global_fvs.rows(); i < n_models; i++)
	{
		vector<Eigen::VectorXf> m_shape_fv(5);
		for (auto shape_d : shape_fvs)
			m_shape_fv.push_back(shape_d.row(i));

		float dist = distance::distance(q_global_fv, global_fvs.row(i), q_shape_fv, m_shape_fv, shape_dists_sd);

		dists.push_back(dist);

		// // Go through all shape descriptors
		// float shape_dist_2 = 0;
		// for (size_t d_i = 0, n_descriptors = shape_fvs.size(); d_i < n_descriptors; d_i++)
		// {
		// 	Eigen::VectorXf m_shape_fv = shape_fvs[d_i].row(i);
		// 	float emd = utils::EarthMoversDistance(q_shape_fv[i], m_shape_fv);
		// 	emd /= shape_dists_sd(i);
		// 	shape_dist_2 += emd*emd;
		// }

		// float global_dist_2 = (q_global_fv - global_fvs.row(i).transpose()).array().square().sum();
		// dists.push_back(sqrtf(global_dist_2 + shape_dist_2));
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
