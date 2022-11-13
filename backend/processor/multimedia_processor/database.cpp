#include "headers.h"

string Database::database_ = "";
vector<string> Database::filenames_ = vector<string>();
vector<string> Database::p_filenames_ = vector<string>();
nlohmann::json Database::classes_ = nlohmann::json(); 
Eigen::MatrixXf Database::global_fvs_ = Eigen::MatrixXf();
vector<Eigen::MatrixXf> Database::shape_fvs_ = vector<Eigen::MatrixXf>();

void Database::SetDatabase(const std::string database) 
{   
    // Reset everything whend database changes
    if (database_ != database)
    {
        filenames_ = vector<string>();
        p_filenames_ = vector<string>();
        global_fvs_ = Eigen::MatrixXf();
        shape_fvs_ = vector<Eigen::MatrixXf>();
    }

    database_ = database;
}
string Database::GetDatabase()
{
    if (database_ == "")
        cout << "WARNING: Database not set! Use Database::SetDatabase({your_database_location})" << endl;
    return database_;
}

vector<string>& Database::GetFilenames(bool processed)
{
    if (filenames_.size() == 0)
    {
        // Read filenames
        ifstream ifs(GetDatabase() + "/files.json");
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
        ifstream ifs(GetDatabase() + "/classes.json");
        classes_ = nlohmann::json::parse(ifs);
    }
        
    return classes_[file];
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
    ifstream ifs(GetDatabase() + "/feature_descriptors.json");
    nlohmann::json json_ds = nlohmann::json::parse(ifs);

    // Load the global feature vectors
    global_fvs_ = Eigen::MatrixXf(n_models, 8);

    for (int i = 0; i < n_models; i++)
    {
        auto ds = json_ds[filenames[i]];
        global_fvs_.row(i) = Eigen::Matrix<float, 8, 1>(
            ds["area"].get<float>(),
            ds["AABBVolume"].get<float>(),
            ds["volume"].get<float>(),
            ds["compactness"].get<float>(),
            ds["eccentricity"].get<float>(),
            ds["diameter"].get<float>(),
            ds["sphericity"].get<float>(),
            ds["rectangularity"].get<float>()
        );
    }

    // Load shape feature fectors
    vector<string> shape_descriptor_names{ "A3", "D1", "D2", "D3", "D4" };
    // Use the first model in the json to determine the binsize for each shape descriptor
    vector<size_t> shape_descriptor_bincount;
    auto firstModel = json_ds.begin().value();
    for (string sd : shape_descriptor_names)
        shape_descriptor_bincount.push_back(firstModel[sd].size());
    // Create a matrix for each shape descriptor where the rows represent the models
    shape_fvs_ = vector<Eigen::MatrixXf>();
    for (size_t i = 0; i < shape_descriptor_names.size(); i++)
        shape_fvs_.push_back(Eigen::MatrixXf(n_models, shape_descriptor_bincount[i]));

    for (int i = 0; i < n_models; i++)
    {
        auto descriptors = json_ds[filenames[i]];
        for (size_t sd_i = 0; sd_i < shape_descriptor_names.size(); sd_i++) 
        {
            auto descriptor = descriptors[shape_descriptor_names[sd_i]];
            size_t sdj = 0;
            for (auto it2 = descriptor.begin(); it2 != descriptor.end(); ++it2, sdj++)
                shape_fvs_[sd_i](i,sdj) = it2.value().get<float>();
        }
    }
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
    string database = GetDatabase();
    printf_debug("Loading mesh \"%s\" from %s\n", file.c_str(), database.c_str());
    return ReadMesh(database + "/models/" + file);
}
// Write a mesh to a database
void Database::WriteMesh(pmp::SurfaceMesh &mesh, const string file)
{
    printf_debug("Writing mesh \"%s\" to disk\n", file.c_str());
    mesh.write(GetDatabase() + "/models/" + file);
}

void Database::WriteStats(string in, string out, const NormalizationStatistics &beforeStats, const NormalizationStatistics &afterStats)
{
    string database = GetDatabase();
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

void Database::WriteDescriptors(vector<string> filenames, vector<descriptors::GlobalDescriptors> gds, vector<descriptors::ShapeDescriptors> sds)
{
    string database = GetDatabase();
    ifstream ifs(database + "/feature_descriptors.json");
    nlohmann::json jsonDescriptors;
    if (!ifs.fail())
        jsonDescriptors = nlohmann::json::parse(ifs);
    for (size_t i = 0, nFiles = filenames.size(); i < nFiles; i++)
        jsonDescriptors[filenames[i]] = DescriptorsToJSON(gds[i], sds[i]);
    ofstream ofs(database + "/feature_descriptors.json");
    ofs << setw(4) << jsonDescriptors << endl; // TODO: removing setw(4) might improve filesize
    ofs.close();
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
    ofstream ofs(GetDatabase() + "/confusion_matrix.json");
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