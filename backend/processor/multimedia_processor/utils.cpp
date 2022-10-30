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
    // No vertex will remain in place
    VertexMat RandomVertices(VertexMap &verts)
    {
        VertexMat ret = VertexMat();
        for (size_t i = 0, j; i < VERTEX_COUNT; i++)
        {
            do j = utils::RandomVertexIndex();
            while (j == i);
            ret.col(i) = verts.col(j);
        }
        return ret;
    }

    // Map a mesh's point list (aka array of Eigen::Matrix3f) to a VertexMap
    VertexMap GetVertexMap(pmp::SurfaceMesh &mesh)
    {
        assert(mesh.n_vertices() == VERTEX_COUNT);
        pmp::VertexProperty points = mesh.get_vertex_property<pmp::Point>("v:point");
        return VertexMap((float*)(points.data()), 3, VERTEX_COUNT);
    }

    float EarthMoversDistance(Eigen::VectorXf a, Eigen::VectorXf b) 
    {
        auto diff = (a - b).array();
        Eigen::ArrayXf cumulative_sum(a.size(), 1);

        partial_sum(diff.begin(), diff.end(), cumulative_sum.begin(), plus<float>());
        float emd = cumulative_sum.abs().sum();
        return emd;
    }

    Eigen::VectorXf JSONArrayToVector(nlohmann::json json_array)
    {
        Eigen::VectorXf v(json_array.size());
        size_t i = 0;
        for (auto it = json_array.begin(); it != json_array.end(); ++it, i++)
            v(i) = *it;
        return v;
    }
}