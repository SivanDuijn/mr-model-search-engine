#include "headers.h"

namespace eigen_vectors 
{
    // Only works for preprocessed meshes!
    Eigen::Matrix3f GetCovariance(pmp::SurfaceMesh &mesh)
    {
        VertexMap map = utils::GetVertexMap(mesh);
        // We don't have to center the samples
        // since that happened in the preprocessing step
        // https://stackoverflow.com/a/15142446
        return (map * map.transpose()) / float(map.cols() - 1);
    }

    EigenValues GetEigenValues(pmp::SurfaceMesh &mesh)
    {
        Eigen::Matrix3f cov = GetCovariance(mesh);
        Eigen::EigenSolver<Eigen::Matrix3f> solver(cov, false);
        Eigen::Vector3f values = solver.eigenvalues().real();
        EigenIndices indices = GetEigenIndices(values);

        return EigenValues(
            values(indices.major),
            values(indices.medium),
            values(indices.minor)
        );
    }

    EigenVectors GetEigenVectors(pmp::SurfaceMesh &mesh) 
    {
        Eigen::Matrix3f cov = GetCovariance(mesh);
        Eigen::EigenSolver<Eigen::Matrix3f> solver(cov, true);
        Eigen::Matrix3f vectors = solver.eigenvectors().real();
        EigenIndices indices = GetEigenIndices(solver.eigenvalues().real());

        return EigenVectors(
            vectors.col(indices.major),
            vectors.col(indices.medium),
            vectors.col(indices.minor)
        );
    }

    EigenIndices GetEigenIndices(Eigen::Vector3f eigenvalues) 
    {
        Eigen::Vector3f v = eigenvalues;
        int major = 0, medium = 0, minor = 0;
        if (v[0] > v[1])
            if (v[0] > v[2])
                if (v[1] > v[2])
                    major = 0, medium = 1, minor = 2;
                else
                    major = 0, medium = 2, minor = 1;
            else
                major = 2, medium = 0, minor = 1;

        else if (v[1] > v[2])
            if (v[0] > v[2])
                major = 1, medium = 0, minor = 2;
            else
                major = 1, medium = 2, minor = 0;

        else 
            major = 2, medium = 1, minor = 0;

        return EigenIndices(major, medium, minor);
    }
}
