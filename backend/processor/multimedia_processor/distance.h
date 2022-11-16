#pragma once

namespace distance 
{
    float distance(
        const Eigen::VectorXf& g_a,
        const Eigen::VectorXf g_b, 
        const std::vector<Eigen::VectorXf>& s_a, 
        const std::vector<Eigen::VectorXf>& s_b, 
        const Eigen::VectorXf& s_dists_sd);
    float combine_global_shape_distance(float g, float total_s, size_t n_shape_descriptors);
    float global_vf_distance(const Eigen::VectorXf& a,const  Eigen::VectorXf b);
    float shape_vfs_distance(const std::vector<Eigen::VectorXf>& a,const  std::vector<Eigen::VectorXf>& b, const Eigen::VectorXf& dists_sd);
    float shape_vf_distance(const Eigen::VectorXf& a, const Eigen::VectorXf& b);
}