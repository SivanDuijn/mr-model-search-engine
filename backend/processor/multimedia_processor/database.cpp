#include "headers.h"

namespace database 
{
    // Read a mesh from file
    pmp::SurfaceMesh read_mesh(const string file)
    {
        printf_debug("Loading mesh \"%s\" from %s\n", file.c_str());
	    pmp::SurfaceMesh mesh;
	    mesh.read(file);
        return mesh;
    }
    // Read a mesh from a database
    pmp::SurfaceMesh read_mesh(const string database, const string file)
    {
	    printf_debug("Loading mesh \"%s\"", file.c_str(), database.c_str());
        return read_mesh(database + '/' + file);
    }

    // Write a mesh to a database
    void write_mesh(pmp::SurfaceMesh &mesh, const string database, const string file)
    {
	    printf_debug("Writing mesh \"%s\" to disk\n", file.c_str());
	    mesh.write(database + '/' + file);
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