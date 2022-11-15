#include "headers.h"

string Database::database_ = "";
vector<string> Database::filenames_ = vector<string>();
vector<string> Database::p_filenames_ = vector<string>();
nlohmann::json Database::classes_ = nlohmann::json(); 
Eigen::VectorXf Database::global_mean_ = Eigen::VectorXf();
Eigen::VectorXf Database::global_sd_ = Eigen::VectorXf();
Eigen::VectorXf Database::shape_dists_mean_ = Eigen::VectorXf();
Eigen::VectorXf Database::shape_dists_sd_ = Eigen::VectorXf();
Eigen::MatrixXf Database::global_fvs_ = Eigen::MatrixXf();
vector<Eigen::MatrixXf> Database::shape_fvs_ = vector<Eigen::MatrixXf>();
vector<float> Database::dist_matrix_ = vector<float>();

void Database::SetDatabaseDir(const std::string database) 
{   
    // Reset everything whend database changes
    if (database_ != database)
    {
        filenames_ = vector<string>();
        p_filenames_ = vector<string>();
        classes_ = nlohmann::json();
        global_mean_ = Eigen::VectorXf();
        global_sd_ = Eigen::VectorXf();
        shape_dists_mean_ = Eigen::VectorXf();
        shape_dists_sd_ = Eigen::VectorXf();
        global_fvs_ = Eigen::MatrixXf();
        shape_fvs_ = vector<Eigen::MatrixXf>();
        dist_matrix_ = vector<float>();
    }

    database_ = database;
}
string Database::GetDatabaseDir()
{
    if (database_ == "")
        cout << "WARNING: Database not set! Use Database::SetDatabaseDir({your_database_location})" << endl;
    return database_;
}

vector<string>& Database::GetFilenames(bool processed)
{
    if (filenames_.size() == 0)
    {
        // Read filenames
        ifstream ifs(GetDatabaseDir() + "/files.json");
        nlohmann::json files = nlohmann::json::parse(ifs);
        for (nlohmann::json::iterator it = files.begin(); it != files.end(); ++it) 
            for (nlohmann::json::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2)
                filenames_.push_back(*it2);
    }

    if (processed)
    {
        if (p_filenames_.size() == 0)
            for (string file : filenames_)
                p_filenames_.push_back(utils::to_processed(file));
            
        return p_filenames_;
    }
    else 
        return filenames_;
}

string Database::GetClass(const std::string file)
{
    if (classes_.size() == 0)
    {
        ifstream ifs(GetDatabaseDir() + "/classes.json");
        classes_ = nlohmann::json::parse(ifs);
    }
        
    return classes_[file];
}

Eigen::VectorXf& Database::GetGlobalMean()
{
    if (global_mean_.size() == 0)
        LoadFVS();
    return global_mean_;
}
Eigen::VectorXf& Database::GetGlobalSD()
{
    if (global_sd_.size() == 0)
        LoadFVS();
    return global_sd_;
}
Eigen::VectorXf& Database::GetShapeDistsMean()
{
    if (shape_dists_mean_.size() == 0)
        LoadFVS();
    return shape_dists_mean_;
}
Eigen::VectorXf& Database::GetShapeDistsSD()
{
    if (shape_dists_sd_.size() == 0)
        LoadFVS();
    return shape_dists_sd_;
}

// The global descriptors in a matrix where each row is model
Eigen::MatrixXf& Database::GetGlobalFVS()
{
    if (global_fvs_.rows() == 0)
        LoadFVS();
    return global_fvs_;
}

// The shape descriptors, for each descriptor a matrix where each row represents a model
vector<Eigen::MatrixXf>& Database::GetShapeFVS()
{
    if (shape_fvs_.size() == 0)
        LoadFVS();
    return shape_fvs_;
}

// Load both the global and shape feature vectors at once, so we only have to read the json once!
void Database::LoadFVS()
{
    vector<string> filenames = GetFilenames(true);
    int n_models = filenames.size();
    ifstream ifs(GetDatabaseDir() + "/feature_vectors.json");
    if (ifs.fail())
    {
        cout << "{\"error\": \"feature_vectors.json not found!\"}" << endl; 
        return;
    }
    nlohmann::json json_fvs = nlohmann::json::parse(ifs);

    // Load in means and SDs
    global_mean_ = utils::json_array_to_vector(json_fvs["global_mean"]);
	global_sd_ = utils::json_array_to_vector(json_fvs["global_sd"]);
	shape_dists_mean_ = utils::json_array_to_vector(json_fvs["shape_dists_mean"]);
	shape_dists_sd_ = utils::json_array_to_vector(json_fvs["shape_dists_sd"]);

    // Rows represent the models
    global_fvs_ = Eigen::MatrixXf(n_models, 8);
    size_t n_shape_descriptors = 5;
    // Create a matrix for each shape descriptor where the rows represent the models
    shape_fvs_ = vector<Eigen::MatrixXf>();
    // Use the first model in the json to determine the binsize for each shape descriptor
    for (size_t d_i = 0; d_i < n_shape_descriptors; d_i++)
        shape_fvs_.push_back(Eigen::MatrixXf(n_models, json_fvs["models"].begin().value()["shape"][d_i].size()));

    for (int i = 0; i < n_models; i++)
    {
        auto file_fvs = json_fvs["models"][filenames[i]];
        global_fvs_.row(i) = utils::json_array_to_vector(file_fvs["global"]);

        for (size_t d_i = 0; d_i < n_shape_descriptors; d_i++) 
            shape_fvs_[d_i].row(i) = utils::json_array_to_vector(file_fvs["shape"][d_i]);
    }

    // // Load shape feature fectors
    // vector<string> shape_descriptor_names{ "A3", "D1", "D2", "D3", "D4" };
    // // Use the first model in the json to determine the binsize for each shape descriptor
    // vector<size_t> shape_descriptor_bincount;
    // auto firstModel = json_ds.begin().value();
    // for (string sd : shape_descriptor_names)
    //     shape_descriptor_bincount.push_back(firstModel[sd].size());
    // // Create a matrix for each shape descriptor where the rows represent the models
    // shape_fvs_ = vector<Eigen::MatrixXf>();
    // for (size_t i = 0; i < shape_descriptor_names.size(); i++)
    //     shape_fvs_.push_back(Eigen::MatrixXf(n_models, shape_descriptor_bincount[i]));

    // for (int i = 0; i < n_models; i++)
    // {
    //     auto descriptors = json_ds[filenames[i]];
    //     for (size_t sd_i = 0; sd_i < shape_descriptor_names.size(); sd_i++) 
    //     {
    //         auto descriptor = descriptors[shape_descriptor_names[sd_i]];
    //         size_t sdj = 0;
    //         for (auto it2 = descriptor.begin(); it2 != descriptor.end(); ++it2, sdj++)
    //             shape_fvs_[sd_i](i,sdj) = it2.value().get<float>();
    //     }
    // }
}

vector<float>& Database::GetDistMatrix()
{
    if (dist_matrix_.size() == 0)
    {
        size_t n_models = GetFilenames().size();

        // Read in distance matrix
        ifstream ifs(GetDatabaseDir() + "/dist_matrix.json");
        nlohmann::json json_dist_matrix;
        if (ifs.fail())
        {
            cout << "{\"error\": \"dist_matrix.json not found!\"}" << endl; 
            return dist_matrix_;
        }
        json_dist_matrix = nlohmann::json::parse(ifs);
        ifs.close();

        dist_matrix_ = vector<float>((n_models * (n_models-1)) / 2);
        int i = 0;
        for (auto it = json_dist_matrix.begin(); it != json_dist_matrix.end(); ++it, i++) 
            dist_matrix_[i] = *it;
    }

    return dist_matrix_;
}

// Read a mesh from file
pmp::SurfaceMesh Database::ReadMesh(const string file)
{
    printf_debug("Loading mesh \"%s\"\n", file.c_str());
    pmp::SurfaceMesh mesh;
    mesh.read(file);
    return mesh;
}
// Read a mesh from a database
pmp::SurfaceMesh Database::ReadMeshFromDatabase(const string file)
{
    string database = GetDatabaseDir();
    printf_debug("Loading mesh \"%s\" from %s\n", file.c_str(), database.c_str());
    return ReadMesh(database + "/models/" + file);
}
// Write a mesh to a database
void Database::WriteMesh(pmp::SurfaceMesh &mesh, const string file)
{
    printf_debug("Writing mesh \"%s\" to disk\n", file.c_str());
    mesh.write(GetDatabaseDir() + "/models/" + file);
}

void Database::WriteFVS(
        Eigen::MatrixXf global_fvs,
        std::vector<descriptors::ShapeDescriptors> shape_fvs,
        Eigen::VectorXf global_mean,
        Eigen::VectorXf global_sd,
        Eigen::VectorXf shape_dists_mean,
        Eigen::VectorXf shape_dists_sd
    )
{
    vector<string> filenames = GetFilenames(true);
    size_t n_models = filenames.size();

    // Write feature vectors and mean and sd to json
	// separate global descriptors and shape descriptors
	nlohmann::json json_fvs;
	nlohmann::json json_models;
	for (int i = 0; i < n_models; i++)
	{
		json_models[filenames[i]] = {
			{"global", global_fvs.row(i).array()},
			{"shape", {
             		shape_fvs[i].A3.bins.array(),
                    shape_fvs[i].D1.bins.array(),
                    shape_fvs[i].D2.bins.array(),
                    shape_fvs[i].D3.bins.array(),
                    shape_fvs[i].D4.bins.array()
                }
            }
		};
	}
	json_fvs["global_mean"] = global_mean.array();
	json_fvs["global_sd"] = global_sd.array();
	json_fvs["shape_dists_mean"] = shape_dists_mean.array();
	json_fvs["shape_dists_sd"] = shape_dists_sd.array();
	json_fvs["models"] = json_models;
	ofstream ofs(GetDatabaseDir() + "/feature_vectors.json");
	ofs << setw(4) << json_fvs << endl;
	ofs.close();
}

void Database::WriteDistMatrix(vector<float>& dist_matrix)
{
    nlohmann::json json_dists = dist_matrix;
	ofstream ofs(GetDatabaseDir() + "/dist_matrix.json");
	ofs << json_dists << endl;
	ofs.close();
}

void Database::WriteStats(string in, string out, const NormalizationStatistics &beforeStats, const NormalizationStatistics &afterStats)
{
    string database = GetDatabaseDir();
    // Write the normalization stats to json
    ifstream ifs(database + "/normalization_stats.json");
    nlohmann::json normStats;
    if (!ifs.fail())
        normStats = nlohmann::json::parse(ifs);
    normStats[in] = StatsToJSON(beforeStats);
    normStats[out] = StatsToJSON(afterStats);
    ofstream ofs(database + "/normalization_stats.json");
    ofs << setw(4) << normStats << endl;
}

void Database::WriteConfusionMatrix(const map<string, map<string, int>> confusion)
{
    vector<string> classes;
    for (auto it = confusion.begin(), end = confusion.end(); it != end; it++)
        classes.push_back(it->first);

    int size = confusion.size();
    vector<vector<int>> output;
    for (auto it = confusion.begin(), end = confusion.end(); it != end; it++)
    {
        vector<int> row;
        for (int i = 0; i < size; i++)
        {
            if (it->second.count(classes[i]) == 1)
            {
                auto mapmap = it->second;
                row.push_back(mapmap[classes[i]]);
            }
            else 
                row.push_back(0);
        }
        output.push_back(row);
    }

    nlohmann::json output_json;
    output_json["matrix"] = output;
    output_json["class_names"] = classes;
    ofstream ofs(GetDatabaseDir() + "/confusion_matrix.json");
    ofs << output_json << endl;
    ofs.close();
}

nlohmann::json Database::StatsToJSON(const NormalizationStatistics &stats)
{
    nlohmann::json json_stats =
    { 
        {"nVertices", stats.nVertices}, 
        {"nFaces", stats.nFaces},
        {"aabbSize", stats.boundingBoxSize},
        {"position", stats.distBarycenterToOrigin},
        {"angleX", stats.angleX},
        {"angleY", stats.angleY},
        {"angleZ", stats.angleZ},
        {"totalAngle", stats.totalAngle},
        {"totalFlip", stats.totalFlip}
    };
    return json_stats;
}

nlohmann::json Database::DescriptorsToJSON(descriptors::GlobalDescriptors &gd, descriptors::ShapeDescriptors &sd)
{
    nlohmann::json json;
    json = {
        {"area", gd.surfaceArea}, 
        {"AABBVolume", gd.AABBVolume},
        {"volume", gd.volume},
        {"compactness", gd.compactness},
        {"eccentricity", gd.eccentricity},
        {"diameter", gd.diameter},
        {"sphericity", gd.sphericity},
        {"rectangularity", gd.rectangularity},
        {"A3", sd.A3.bins.array()},
        {"D1", sd.D1.bins.array()},
        {"D2", sd.D2.bins.array()},
        {"D3", sd.D3.bins.array()},
        {"D4", sd.D4.bins.array()}
    };
    return json;
}