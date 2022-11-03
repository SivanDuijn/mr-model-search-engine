#include "headers.h"

namespace database 
{
    // Read a mesh from a database
    pmp::SurfaceMesh read_mesh(const string database, const string file)
    {
	    printf_debug("Loading mesh \"%s\" from %s\n", file.c_str(), database.c_str());
	    pmp::SurfaceMesh mesh;
	    mesh.read(database + '/' + file);
        return mesh;
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