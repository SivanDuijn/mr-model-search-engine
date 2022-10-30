#pragma once

namespace eigen_vectors 
{
    struct EigenVectors
    {
        Eigen::Vector3f major;
        Eigen::Vector3f medium;
        Eigen::Vector3f minor;
    };
    struct EigenValues
    {
        float major;
        float medium;
        float minor;
    };
    struct EigenIndices
    {
        int major;
        int medium;
        int minor;
    };

    EigenValues GetEigenValues(pmp::SurfaceMesh &mesh);
    EigenVectors GetEigenVectors(pmp::SurfaceMesh &mesh);
    EigenIndices GetEigenIndices(Eigen::Vector3f eigenvalues);
}
