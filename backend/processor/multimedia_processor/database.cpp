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
        return read_mesh(database + '/' + file);
    }

    // Write a mesh to a database
    void write_mesh(pmp::SurfaceMesh &mesh, const string database, const string file)
    {
	    printf_debug("Writing mesh \"%s\" to disk\n", file.c_str());
	    mesh.write(database + '/' + file);
    }
    
    void write_stats(string database, string in, string out, const NormalizationStatistics &beforeStats, const NormalizationStatistics &afterStats)
    {
        // Write the normalization stats to json
        ifstream ifs(database + "/normalization_stats.json");
        nlohmann::json normStats;
        if (!ifs.fail())
            normStats = nlohmann::json::parse(ifs);
        normStats[in] = { 
            {"nVertices", beforeStats.nVertices}, 
            {"nFaces", beforeStats.nFaces},
            {"aabbSize", beforeStats.boundingBoxSize},
            {"position", beforeStats.distBarycenterToOrigin},
            {"angleX", beforeStats.angleX},
            {"angleY", beforeStats.angleY},
            {"angleZ", beforeStats.angleZ},
            {"totalAngle", beforeStats.totalAngle},
            {"totalFlip", beforeStats.totalFlip}
        };
        normStats[out] = { 
            {"nVertices", afterStats.nVertices}, 
            {"nFaces", afterStats.nFaces},
            {"aabbSize", afterStats.boundingBoxSize},
            {"position", afterStats.distBarycenterToOrigin},
            {"angleX", afterStats.angleX},
            {"angleY", afterStats.angleY},
            {"angleZ", afterStats.angleZ},
            {"totalAngle", afterStats.totalAngle},
            {"totalFlip", afterStats.totalFlip}
        };
        ofstream ofs(database + "/normalization_stats.json");
        ofs << setw(4) << normStats << endl;
    }

    void write_descriptors(string database, vector<string> filenames, vector<descriptors::GlobalDescriptors> gds, vector<descriptors::ShapeDescriptors> sds)
    {
        ifstream ifs(database + "/feature_descriptors.json");
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
	    ofstream ofs(database + "/feature_descriptors.json");
	    ofs << setw(4) << jsonDescriptors << endl; // TODO: removing setw(4) might improve filesize
	    ofs.close();
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
}