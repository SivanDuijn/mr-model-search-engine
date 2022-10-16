#pragma once

namespace eigen_vectors 
{
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> GetMajorMinorEigenVectors(Eigen::Matrix3f &cov);
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> GetMajorMinorEigenVectors(pmp::SurfaceMesh &mesh);
    std::tuple<int, int, int> GetMajorMinorIndexEigenValues(Eigen::Vector3f eigenvalues);
}
