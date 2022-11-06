#include "headers.h"

namespace database 
{
    // Read a mesh from file
    pmp::SurfaceMesh read_mesh(const string file)
    {
        printf_debug("Loading mesh \"%s\"\n", file.c_str());
	    pmp::SurfaceMesh mesh;
	    mesh.read(file);
        return mesh;
    }
    // Read a mesh from a database
    pmp::SurfaceMesh read_mesh(const string database, const string file)
    {
	    printf_debug("Loading mesh \"%s\" from %s\n", file.c_str(), database.c_str());
        return read_mesh(database + "/models/" + file);
    }

    // Write a mesh to a database
    void write_mesh(pmp::SurfaceMesh &mesh, const string database, const string file)
    {
	    printf_debug("Writing mesh \"%s\" to disk\n", file.c_str());
	    mesh.write(database + "/models/" + file);
    }
    
    void write_stats(string database, string in, string out, const NormalizationStatistics &beforeStats, const NormalizationStatistics &afterStats)
    {
        // Write the normalization stats to json
        ifstream ifs(database + "/normalization_stats.json");
        nlohmann::json normStats;
        if (!ifs.fail())
            normStats = nlohmann::json::parse(ifs);
        normStats[in] = stats_to_json(beforeStats);
        normStats[out] = stats_to_json(afterStats);
        ofstream ofs(database + "/normalization_stats.json");
        ofs << setw(4) << normStats << endl;
    }
    
    nlohmann::json stats_to_json(const NormalizationStatistics &stats)
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

    void write_descriptors(string database, vector<string> filenames, vector<descriptors::GlobalDescriptors> gds, vector<descriptors::ShapeDescriptors> sds)
    {
        ifstream ifs(database + "/feature_descriptors.json");
	    nlohmann::json jsonDescriptors;
	    if (!ifs.fail())
	    	jsonDescriptors = nlohmann::json::parse(ifs);
	    for (size_t i = 0, nFiles = filenames.size(); i < nFiles; i++)
	    	jsonDescriptors[filenames[i]] = descriptors_to_json(gds[i], sds[i]);
	    ofstream ofs(database + "/feature_descriptors.json");
	    ofs << setw(4) << jsonDescriptors << endl; // TODO: removing setw(4) might improve filesize
	    ofs.close();
    }
    
    nlohmann::json descriptors_to_json(descriptors::GlobalDescriptors &gd, descriptors::ShapeDescriptors &sd)
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

	vector<string> get_filenames(const string database, const bool processed)
    {
		vector<string> filenames = vector<string>();
        // Read filenames
        ifstream ifs(database + "/files.json");
        nlohmann::json files = nlohmann::json::parse(ifs);
        for (nlohmann::json::iterator it = files.begin(); it != files.end(); ++it) 
            for (nlohmann::json::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2)
            {
                string file(*it2);
                if (processed)
                {
                    size_t pos = file.find('.');
                    string name = file.substr(0, pos);
                    string ext = file.substr(pos + 1);
                    // Calculate descriptors for all the processed models
                    file = name + "_processed." + ext;
                }
                filenames.push_back(file);
            }
		return filenames;
    }

    // Retrieves the global descriptors in a matrix where each row is model
    Eigen::MatrixXf read_all_global_fvs(const string database, const vector<string> &filenames)
    {
	    int n_models = filenames.size();
        ifstream ifs(database + "/feature_descriptors.json");
	    nlohmann::json json_ds = nlohmann::json::parse(ifs);

        Eigen::MatrixXf global_fvs(n_models, 8);

        for (int i = 0; i < n_models; i++)
        {
            auto ds = json_ds[filenames[i]];
            global_fvs.row(i) = Eigen::Matrix<float, 8, 1>(
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
        return global_fvs;
    }

    // Retrieves the shape descriptors, for each descriptor a matrix where each row represents a model
    vector<Eigen::MatrixXf> read_all_shape_fvs(const string database, const vector<string> &filenames)
    {
	    int n_models = filenames.size();
        ifstream ifs(database + "/feature_descriptors.json");
	    nlohmann::json json_ds = nlohmann::json::parse(ifs);

        vector<string> shape_descriptor_names{ "A3", "D1", "D2", "D3", "D4" };
        // Use the first model in the json to determine the binsize for each shape descriptor
        vector<size_t> shape_descriptor_bincount;
        auto firstModel = json_ds.begin().value();
        for (string sd : shape_descriptor_names)
            shape_descriptor_bincount.push_back(firstModel[sd].size());
        // Create a matrix for each shape descriptor where the rows represent the models
        vector<Eigen::MatrixXf> shape_fvs;
        for (size_t i = 0; i < shape_descriptor_names.size(); i++)
            shape_fvs.push_back(Eigen::MatrixXf(n_models, shape_descriptor_bincount[i]));

        for (int i = 0; i < n_models; i++)
        {
            auto descriptors = json_ds[filenames[i]];
            for (size_t sd_i = 0; sd_i < shape_descriptor_names.size(); sd_i++) 
            {
                auto descriptor = descriptors[shape_descriptor_names[sd_i]];
                size_t sdj = 0;
                for (auto it2 = descriptor.begin(); it2 != descriptor.end(); ++it2, sdj++)
                    shape_fvs[sd_i](i,sdj) = it2.value().get<float>();
            }
        }
        return shape_fvs;
    }
    
}