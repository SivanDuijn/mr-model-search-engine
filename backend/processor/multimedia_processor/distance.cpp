#include "headers.h"

namespace distance 
{
    // PRIVATE ACTUAL DISTANCE FUNCTIONS
    namespace {
        float euclidian_distance(const Eigen::VectorXf& a,const  Eigen::VectorXf b)
        {
            // return sqrtf((a - b).array().square().sum());
            return (a - b).array().square().sum();
        }

        float earth_movers_distance(const Eigen::VectorXf& a,const  Eigen::VectorXf& b)
        {
            Eigen::ArrayXf diff = (a - b).array();
            Eigen::ArrayXf cumulative_sum(a.size(), 1);

            partial_sum(diff.begin(), diff.end(), cumulative_sum.begin(), plus<float>());
            return cumulative_sum.abs().sum();
        }
    }

    float distance(
        const Eigen::VectorXf& g_a,
        const Eigen::VectorXf g_b, 
        const vector<Eigen::VectorXf>& s_a, 
        const vector<Eigen::VectorXf>& s_b, 
        const Eigen::VectorXf& s_dists_sd)
    {
        float g = global_vf_distance(g_a, g_b);
        float s = shape_vfs_distance(s_a, s_b, s_dists_sd);
        
        // average by the number of distances, the shape distance is actually 5 distances summed up
        float total = combine_global_shape_distance(g, s, s_a.size());
    }

    float combine_global_shape_distance(float g, float total_s, size_t n_shape_descriptors)
    {
        // return (g + total_s) / (1 + n_shape_descriptors);
        return sqrtf(g + total_s);
    }

    float global_vf_distance(const Eigen::VectorXf& a, const Eigen::VectorXf b)
    {
        return euclidian_distance(a,b);
    }

    float shape_vf_distance(const Eigen::VectorXf& a, const Eigen::VectorXf& b)
    {
        return earth_movers_distance(a, b);
    }

    float shape_vfs_distance(const vector<Eigen::VectorXf>& a, const vector<Eigen::VectorXf>& b, const Eigen::VectorXf& dists_sd)
    {
        float total = 0;
        for (size_t i = 0, n_descriptors = a.size(); i < n_descriptors; i++)
            total += shape_vf_distance(a[i], b[i]) / dists_sd(i);
        return total;
    }
}