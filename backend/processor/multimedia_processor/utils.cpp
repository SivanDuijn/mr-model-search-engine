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

    float EarthMoversDistance(Eigen::VectorXf a, Eigen::VectorXf b) 
    {
        auto diff = (a - b).array();
        Eigen::ArrayXf cumulative_sum(a.size(), 1);

        partial_sum(diff.begin(), diff.end(), cumulative_sum.begin(), plus<float>());
        float emd = cumulative_sum.abs().sum();
        return emd;
    }

    Eigen::VectorXf json_array_to_vector(nlohmann::json json_array)
    {
        Eigen::VectorXf v(json_array.size());
        size_t i = 0;
        for (auto it = json_array.begin(); it != json_array.end(); ++it, i++)
            v(i) = *it;
        return v;
    }

    string mesh_to_off_string(const pmp::SurfaceMesh &mesh)
    {
        string out = "OFF\n";
        out += to_string(mesh.n_vertices()) + " " + to_string(mesh.n_faces()) + " " + to_string(mesh.n_edges()) + "\n";
        auto points = mesh.get_vertex_property<pmp::Point>("v:point");
        for (auto v: mesh.vertices())
        {
            auto p = points[v];
            out += to_string(p[0]) + " " + to_string(p[1]) + " " + to_string(p[2]) + "\n";
        }

        for (auto f: mesh.faces())
        {
            auto fv = mesh.vertices(f);
            auto fvend = fv;
            out += "3 ";
            do
            {
                out += to_string((*fv).idx()) + " ";
            } while (++fv != fvend);
            out += "\n";
        }

        return out;
    }

    string to_processed(const string file) 
    {
        size_t pos = file.find('.');
        string name = file.substr(0, pos);
        string ext = file.substr(pos + 1);
        // Calculate descriptors for all the processed models
        return name + "_processed." + ext;
    }
}