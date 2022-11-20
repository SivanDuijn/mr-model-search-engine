#include "headers.h"

namespace distance 
{
    // PRIVATE ACTUAL DISTANCE FUNCTIONS
    namespace {
        float cosine_distance(const Eigen::VectorXf& a, const Eigen::VectorXf& b)
        {
            return abs(a.dot(b) / (a.norm() * b.norm()));
        }

        float euclidian_distance(const Eigen::VectorXf& a,const Eigen::VectorXf& b)
        {
            return sqrtf((a - b).array().square().sum());
            // return (a - b).array().square().sum();
        }

        float earth_movers_distance(const Eigen::VectorXf& a,const Eigen::VectorXf& b)
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
        float total = (g*3 + s) / (3 + s_a.size());
        // float total = sqrtf(g + s);

        return total;
    }

    float global_vf_distance(const Eigen::VectorXf& a, const Eigen::VectorXf b)
    {
        // Eigen::ArrayXf weights {{3, 1, 1, 2}};
        Eigen::ArrayXf weights {{1, 1, 1, 1, 1, 1, 1, 1}};
        weights = weights / weights.sum() * N_GLOBAL_FEATURES;

        return euclidian_distance(a.array() * weights, b.array() * weights);
        // return cosine_distance(a.array() * weights, b.array() * weights);
    }

    float shape_vf_distance(const Eigen::VectorXf& a, const Eigen::VectorXf& b)
    {
        return earth_movers_distance(a, b);
    }

    float shape_vfs_distance(const vector<Eigen::VectorXf>& a, const vector<Eigen::VectorXf>& b, const Eigen::VectorXf& dists_sd)
    {
        Eigen::ArrayXf weights {{2, 4, 1, 1, 1}};
        weights = weights / weights.sum() * 5;

        float total = 0;
        for (size_t i = 0, n_descriptors = a.size(); i < n_descriptors; i++)
            total += (shape_vf_distance(a[i], b[i]) / dists_sd(i)) * weights(i);
        return total;
    }
}