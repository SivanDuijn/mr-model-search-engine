#include "headers.h"

namespace database 
{
    // Read a mesh from a database
    pmp::SurfaceMesh read_mesh(const string database, const string file)
    {
	    printf_debug("Loading mesh \"%s\" from %s\n", file.c_str(), database.c_str());
	    pmp::SurfaceMesh mesh;
	    mesh.read(vars::GetAssetPath(database + "/models/" + file));
        return mesh;
    }

    // Write a mesh to a database
    void write_mesh(pmp::SurfaceMesh &mesh, const string database, const string file)
    {
	    printf_debug("Writing mesh \"%s\" to disk\n", file.c_str());
	    mesh.write(vars::GetAssetPath(database + "/models/" + file));
    }

	vector<string> get_filenames(const string database, const bool processed)
    {
		vector<string> filenames = vector<string>();
        // Read filenames
        ifstream ifs(vars::GetAssetPath(database + "/files.json"));
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