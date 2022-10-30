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

    Eigen::VectorXf JSONArrayToVector(nlohmann::json json_array)
    {
        Eigen::VectorXf v(json_array.size());
        size_t i = 0;
        for (auto it = json_array.begin(); it != json_array.end(); ++it, i++)
            v(i) = *it;
        return v;
    }
}