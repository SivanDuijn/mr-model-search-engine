#pragma once

#include <string>
#include <pmp/SurfaceMesh.h>
#include <Eigen/Dense>

#if 1
    #define printf_debug printf
#else
    #define printf_debug 0 && printf
#endif

namespace utils 
{
    Eigen::Map<Eigen::MatrixXf> GetVertexMap(pmp::SurfaceMesh &mesh);

    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> GetMajorMinorEigenVectors(Eigen::Matrix3f cov);
    std::tuple<int, int> GetMajorMinorIndexEigenValues(Eigen::Vector3f eigenvalues);
}
