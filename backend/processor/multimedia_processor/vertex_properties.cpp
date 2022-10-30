#include "headers.h"

namespace VertexProperties 
{
    // Map a mesh's point list (aka array of Eigen::Matrix3f) to a VertexMap
    VertexMap GetVertexMap(pmp::SurfaceMesh &mesh)
    {
        assert(mesh.n_vertices() == VERTEX_COUNT);
        pmp::VertexProperty points = mesh.get_vertex_property<pmp::Point>("v:point");
        return VertexMap((float*)(points.data()), 3, VERTEX_COUNT);
    }

    // Get a random vertex index
    // Only works for preprocessed meshes!
    unsigned int RandomVertexIndex()
    {
        return utils::RandomUInt() & (VERTEX_COUNT - 1);
    }

    // Get an array of random vertices from a resampled mesh
    // No vertex will remain in place
    VertexMat RandomVertices(VertexMap &verts)
    {
        VertexMat ret = VertexMat();
        for (size_t i = 0, j; i < VERTEX_COUNT; i++)
        {
            do j = RandomVertexIndex();
            while (j == i);
            ret.col(i) = verts.col(j);
        }
        return ret;
    }

    // Get the covariance matrix of a set of vertices
    // Only works for preprocessed meshes!
    Eigen::Matrix3f GetCovariance(VertexMap &verts)
    {
        // We don't have to center the samples
        // since that happened in the preprocessing step
        // https://stackoverflow.com/a/15142446
        return (verts * verts.transpose()) / float(verts.cols() - 1);
    }

    Eigen::Vector3i GetEigenIndices(Eigen::Vector3f eigenvalues) 
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

        return Eigen::Vector3i(major, medium, minor);
    }

    EigenValues GetEigenValues(VertexMap &verts)
    {
        Eigen::Matrix3f cov = GetCovariance(verts);
        Eigen::EigenSolver<Eigen::Matrix3f> solver(cov, false);
        Eigen::Vector3f values = solver.eigenvalues().real();
        Eigen::Vector3i indices = GetEigenIndices(values);

        return EigenValues(
            values(indices(0)),
            values(indices(1)),
            values(indices(2))
        );
    }

    EigenVectors GetEigenVectors(VertexMap &verts) 
    {
        Eigen::Matrix3f cov = GetCovariance(verts);
        Eigen::EigenSolver<Eigen::Matrix3f> solver(cov, true);
        Eigen::Matrix3f vectors = solver.eigenvectors().real();
        Eigen::Vector3f values = solver.eigenvalues().real();
        Eigen::Vector3i indices = GetEigenIndices(values);

        return EigenVectors(
            vectors.col(indices(0)),
            vectors.col(indices(1)),
            vectors.col(indices(2))
        );
    }
}
