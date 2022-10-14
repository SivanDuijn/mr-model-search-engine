#include "headers.h"

namespace eigen_vectors 
{
    // Map the mesh's point list (aka array of Eigen::Matrix3f) to a single Eigen::MatrixXd
    Eigen::Map<Eigen::MatrixXf> GetVertexMap(pmp::SurfaceMesh &mesh)
    {
        pmp::VertexProperty points = mesh.get_vertex_property<pmp::Point>("v:point");
        return Eigen::Map<Eigen::MatrixXf>((float*)(points.data()), 3, mesh.n_vertices());
    }

    std::tuple<int, int> GetMajorMinorIndexEigenValues(Eigen::Vector3f eigenvalues) 
    {
        Eigen::Vector3f v = eigenvalues;
        int majorI = 0; 
        int minorI = 0;
        if (v[0] > v[1])
            if (v[0] > v[2])
            {
                majorI = 0;
                if (v[1] > v[2])
                    minorI = 1;
                else 
                    minorI = 2;
            }
            else
            {
                majorI = 2;
                minorI = 0;
                // 2 > 0 > 1
            } 
        else if (v[1] > v[2])
        {
            // 1 > 0, 1 > 2
            majorI = 1;
            if (v[0] > v[2])
                minorI = 0;
            else 
                minorI = 2;
        }
        else 
        {
            majorI = 2;
            minorI = 1;
            // 2 > 1 > 0
        }

        return std::make_tuple(majorI, minorI);
    }

    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> GetMajorMinorEigenVectors(pmp::SurfaceMesh &mesh) 
    {
        Eigen::Map<Eigen::MatrixXf> map = GetVertexMap(mesh);
        Eigen::Matrix3f cov = (map * map.transpose()) / float(map.cols() - 1);
        return GetMajorMinorEigenVectors(cov);
    }
    
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> GetMajorMinorEigenVectors(Eigen::Matrix3f &cov)
    {
        Eigen::EigenSolver<Eigen::MatrixXf> solver;
        solver.compute(cov);

        Eigen::MatrixXf eigen = solver.eigenvectors().real();
        std::tuple<int, int> eigenIndices = GetMajorMinorIndexEigenValues(solver.eigenvalues().real());
        Eigen::Vector3f	major = eigen.col(get<0>(eigenIndices));
        Eigen::Vector3f minor = eigen.col(get<1>(eigenIndices));
        Eigen::Vector3f cross = major.cross(minor);

        return std::make_tuple(major, minor, cross);
    }
}