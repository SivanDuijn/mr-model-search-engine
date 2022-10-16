#include "headers.h"

static unsigned int seed = 0x12345678;

namespace utils
{
    // Get a random uint
    // (Marsaglia's xor32)
    unsigned int RandomUInt()
    {
    	seed ^= seed << 13;
    	seed ^= seed >> 17;
    	seed ^= seed << 5;
    	return seed;
    }
    // Get a random vertex index
    // NOTE: only for resampled meshes
    unsigned int RandomVertexIndex()
    {
        return RandomUInt() & (VERTEX_COUNT - 1);
    }
    // Get an array of random vertices from a resampled mesh
    VertexMat RandomVertices(VertexMap &verts)
    {
        VertexMat ret = VertexMat();
        for (size_t i = 0; i < VERTEX_COUNT; i++)
            ret.col(i) = verts.col(utils::RandomVertexIndex());
        return ret;
    }

    // Map a mesh's point list (aka array of Eigen::Matrix3f) to a VertexMap
    VertexMap GetVertexMap(pmp::SurfaceMesh &mesh)
    {
        assert(mesh.n_vertices() == VERTEX_COUNT);
        pmp::VertexProperty points = mesh.get_vertex_property<pmp::Point>("v:point");
        return VertexMap((float*)(points.data()), 3, VERTEX_COUNT);
    }
    
    void GetAllFilenamesInDatabase(const std::string database, std::vector<std::string>& filenames)
    {
        // Read in filenames
        ifstream ifs(vars::GetAssetPath(database + "/files.json"));
        nlohmann::json files = nlohmann::json::parse(ifs);
        for (nlohmann::json::iterator it = files.begin(); it != files.end(); ++it) 
            for (nlohmann::json::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2) 
                filenames.push_back(*it2);
    }
}