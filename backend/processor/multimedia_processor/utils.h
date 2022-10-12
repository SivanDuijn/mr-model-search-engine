#pragma once

#include <string>
#include <Eigen/Dense>

#ifdef DEBUG
    #define printf_debug printf
#else
    #define printf_debug __noop
#endif

namespace utils 
{
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> GetMajorMinorEigenVectors(Eigen::Matrix3f cov);
    std::tuple<int, int> GetMajorMinorIndexEigenValues(Eigen::Vector3f eigenvalues);
}
